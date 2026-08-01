#include "LibraryPch.h"
#include "IocpClient.h"

void jh::IocpClient::ProcessRecv(Session* sessionPtr, DWORD transferredBytes)
{
	if (false == sessionPtr->m_recvBuffer.MoveRear(transferredBytes))
	{
		_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L" Recvbuffer Moverear failed. SessionId : [0x%016llx]", sessionPtr->m_ullSessionId);

		Disconnect(sessionPtr->m_ullSessionId);

		return;
	}

	while (1)
	{
		int bufferSize = sessionPtr->m_recvBuffer.GetUseSize();

#ifdef ECHO
		if (bufferSize < sizeof(PacketHeader::size))
			break;

		USHORT header;

		sessionPtr->m_recvBuffer.PeekRetBool(reinterpret_cast<char*>(&header), sizeof(header));

		if (bufferSize < (sizeof(header) + header))
			break;

		sessionPtr->m_recvBuffer.MoveFront(sizeof(header));

		PacketBufferRef packet = jh::MakeShared<PacketBuffer>(header);

		if (false == sessionPtr->m_recvBuffer.DequeueRetBool(packet->GetRearPtr(), header))
		{
			_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L" Recvbuffer Dequeue failed. SessionId : [0x%016llx]", sessionPtr->m_ullSessionId);

			Disconnect(sessionPtr->m_ullSessionId);

			return;
		}

		packet->MoveRearPos(header);

		OnRecv(sessionPtr->m_ullSessionId, packet, 0);

#else
		if (bufferSize < sizeof(PacketHeader))
			break;

		PacketHeader header;

		sessionPtr->m_recvBuffer.PeekRetBool(reinterpret_cast<char*>(&header), sizeof(PacketHeader));


		if (bufferSize < (sizeof(PacketHeader) + header.size))
			break;

		sessionPtr->m_recvBuffer.MoveFront(sizeof(header));

		PacketBufferRef packet = jh::MakeShared<PacketBuffer>(header.size);

		if (false == sessionPtr->m_recvBuffer.DequeueRetBool(packet->GetRearPtr(), header.size))
		{
			_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L" Recvbuffer Dequeue failed. SessionId : [0x%016llx]", sessionPtr->m_ullSessionId);

			Disconnect(sessionPtr->m_ullSessionId);

			return;
		}

		packet->MoveRearPos(header.size);

		OnRecv(sessionPtr->m_ullSessionId, packet, header.type);
#endif
		InterlockedIncrement(&m_lRecvCount);
	}

	PostRecv(sessionPtr);
}

void jh::IocpClient::ProcessSend(Session* sessionPtr, DWORD transferredBytes)
{
	size_t pendingCnt = sessionPtr->m_sendOverlapped.m_pendingList.size();

	InterlockedAdd(&m_lSendCount, pendingCnt);
	sessionPtr->m_sendOverlapped.ClearPendingList();

	InterlockedExchange8(&sessionPtr->m_bSendFlag, 0);

	PostSend(sessionPtr);

	return;
}

void jh::IocpClient::Disconnect(ULONGLONG sessionId)
{
	Session* sessionPtr = TryAcquireSession(sessionId);

	if (nullptr == sessionPtr)
		return;

	// 연결 끊긴 상태가 아닌, 먼저 연결을 끊는 상황에서
	// 모두 완료 통지가 들어왔을 때 Disconnect()가 호출된다면
	// 그다음 완료 통지를 처리할 때 i/o 등록을 막는다.
	InterlockedExchange8(&sessionPtr->m_bConnectedFlag, 0);

	// recv, send가 등록되었다면 둘 다 io 취소한다.
	CancelIoEx(reinterpret_cast<HANDLE>(sessionPtr->m_socket), nullptr);

	DecreaseIoCount(sessionPtr);
}

void jh::IocpClient::PostSend(Session* sessionPtr)
{
	if (InterlockedAnd8(&sessionPtr->m_bConnectedFlag, 1) == 0)
		return;


	if (InterlockedExchange8(&sessionPtr->m_bSendFlag, 1) == 1)
		return;

	// 앞에서 막는 것만으로는 모두 막을 수 없음.
	// 0개가 아님을 확인하고 들어오는 순간에 0개가 될 수 있음.
	if (sessionPtr->m_sendQ.GetUseSize() == 0)
	{
		InterlockedExchange8(&sessionPtr->m_bSendFlag, 0);
		return;
	}

	static thread_local alignas(64) std::queue<PacketBufferRef> tempQ;

	sessionPtr->m_sendQ.Swap(tempQ);

	size_t popCount = tempQ.size();

	while (tempQ.size() > 0)
	{
		sessionPtr->m_sendOverlapped.m_pendingList.push_back(std::move(tempQ.front()));

		tempQ.pop();
	}

	std::vector<WSABUF> wsaBufs;

	wsaBufs.reserve(popCount);

	for (PacketBufferRef& packet : sessionPtr->m_sendOverlapped.m_pendingList)
	{
		wsaBufs.push_back({ static_cast<ULONG>(packet->GetDataSize()), packet->GetFrontPtr() });
	}

	InterlockedIncrement(&sessionPtr->m_lIoCount);

	int sendRet = WSASend(sessionPtr->m_socket, wsaBufs.data(), popCount, nullptr, 0, reinterpret_cast<LPWSAOVERLAPPED>(&sessionPtr->m_sendOverlapped), nullptr);

	if (sendRet == SOCKET_ERROR)
	{
		int gle = WSAGetLastError();

		// 등록에 실패한 상황
		if (gle != WSA_IO_PENDING)
		{
			_LOG(m_pcwszClientName, LOG_LEVEL_WARNING, L"[PostSend] WSASend failed. WSAGetLastError : [%d], SessionID: [0x%016llx]", gle, sessionPtr->m_ullSessionId);

			DecreaseIoCount(sessionPtr);

			return;
		}
	}
}

void jh::IocpClient::PostRecv(Session* sessionPtr)
{
	if (InterlockedAnd8(&sessionPtr->m_bConnectedFlag, 1) == 0)
		return;

	WSABUF buf[2]{};

	int directEnqueueSize = sessionPtr->m_recvBuffer.DirectEnqueueSize();
	int remainderSize = sessionPtr->m_recvBuffer.GetFreeSize() - directEnqueueSize;
	int wsabufSize = 1;

	buf[0].buf = sessionPtr->m_recvBuffer.GetFrontBufferPtr();
	buf[0].len = directEnqueueSize;

	if (remainderSize > 0)
	{
		++wsabufSize;
		buf[1].buf = sessionPtr->m_recvBuffer.GetStartBufferPtr();
		buf[1].len = remainderSize;
	}

	DWORD flag = 0;

	InterlockedIncrement(&sessionPtr->m_lIoCount);

	int recvRet = WSARecv(sessionPtr->m_socket, buf, wsabufSize, nullptr, &flag, reinterpret_cast<LPWSAOVERLAPPED>(&sessionPtr->m_recvOverlapped), nullptr);

	if (recvRet == SOCKET_ERROR)
	{
		int gle = WSAGetLastError();

		if (gle != WSA_IO_PENDING)
		{
			_LOG(m_pcwszClientName, LOG_LEVEL_WARNING, L"[PostRecv] WSARecv failed. WSAGetLastError : [%d], SessionID: [0x%016llx]", gle, sessionPtr->m_ullSessionId);
			DecreaseIoCount(sessionPtr);

			return;
		}
	}
}

void jh::IocpClient::SendPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	Session* sessionPtr = TryAcquireSession(sessionId);

	if (nullptr == sessionPtr)
		return;

	sessionPtr->m_sendQ.Push(packet);

	PostSend(sessionPtr);

	DecreaseIoCount(sessionPtr);
}

unsigned WINAPI jh::IocpClient::WorkerThreadFunc(LPVOID lparam)
{
	IocpClient* instance = reinterpret_cast<IocpClient*>(lparam);

	instance->WorkerThreadMain();

	return 0;
}

void jh::IocpClient::WorkerThreadMain()
{
	while (1)
	{
		LPOVERLAPPED lpOverlapped = nullptr;
		DWORD transferredBytes = 0;
		ULONG_PTR key = 0;

		Session* sessionPtr = nullptr;

		bool gqcsRet = GetQueuedCompletionStatus(m_hCompletionPort, &transferredBytes,
			reinterpret_cast<PULONG_PTR>(&sessionPtr), &lpOverlapped, INFINITE);

		// 
		if (nullptr == lpOverlapped)
		{
			// gqcsRet == true -> pqcs로 보낸 lpOverlapped 가 nullptr
			// gqcsRet == false -> completionPort가 closeHandle된 상태.
			if (false == gqcsRet)
			{
				int gle = WSAGetLastError();

				_LOG(m_pcwszClientName, LOG_LEVEL_INFO, L"[WorkerThreadMain] ProcessIO - Close Completion port [WSAGetLastError : %d]", gle);
			}
			break;
		}

		// lpOverlapped != nullptr, gqcsRet == false
		// 작업 도중 연결이 끊겼을 때의 상황이다.
	/*	if (false == gqcsRet)
		{
			DWORD gle = GetLastError();
			_LOG(L"Client GQCS Failed", LOG_LEVEL_INFO, L"GetLastError - [%u] SessionID : [0x%0x16llx]", gle,sessionPtr);

			InterlockedIncrement(&m_llDisconnectedCount);

			Disconnect(sessionPtr->m_ullSessionId, L"gqcs false");

			DecreaseIoCount(sessionPtr);

			continue;
		}

		DWORD type = &sessionPtr->m_recvOverlapped == lpOverlapped ? TYPE_RECV : (&sessionPtr->m_sendOverlapped == lpOverlapped ? TYPE_SEND : TYPE_CONN);*/


		if (&sessionPtr->m_connectOverlapped == lpOverlapped)
		{
			// connectex 성공인 경우에만. 실패인 경우는 알아서 종료
			if (true == gqcsRet)
			{
				// api사용을 위해 소켓 정보 초기화
				setsockopt(sessionPtr->m_socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);

				OnConnected(sessionPtr->m_ullSessionId);

				InterlockedIncrement64(&m_llTotalConnectedSessionCount);

				PostRecv(sessionPtr);
			}
		}
		else if (transferredBytes != 0)
		{
			if (&sessionPtr->m_recvOverlapped == lpOverlapped)
			{
				ProcessRecv(sessionPtr, transferredBytes);
			}
			else if (&sessionPtr->m_sendOverlapped == lpOverlapped)
			{
				ProcessSend(sessionPtr, transferredBytes);
			}
		}


		DecreaseIoCount(sessionPtr);

		//DecreaseIoCount(sessionPtr);
	}
}

jh::Session* jh::IocpClient::TryAcquireSession(ULONGLONG sessionId)
{
	if (sessionId == INVALID_SESSION_ID)
	{
		_LOG(m_pcwszClientName, LOG_LEVEL_WARNING, L"[TryAcquireSession] Invalid Session ID [0x%016llx]", sessionId);
		return nullptr;
	}

	LONGLONG sessionIdx = sessionId >> SESSION_IDX_SHIFT_BIT;

	Session* sessionPtr = &m_pClientSessionArr[sessionIdx];
	// 세션을 그냥 얻어가면 줬을 때 내가 준 세션이 해제되었을 수 있다. 그래서 ID 확인 후 건네주도록 한다.
	if (sessionId != sessionPtr->m_ullSessionId)
	{
		// 이미 해제된 세션에 접근하는 것은 아주 정상적인 상태.
		_LOG(m_pcwszClientName, LOG_LEVEL_DEBUG, L"[TryAcquireSession] Session ID != _SessionArray[SessionIdx].SessionID, SessionID : [0x%016llx], SessionIdx : [0x%016llx], Caller : [%s]", sessionId, sessionIdx);

		return nullptr;
	}

	// 받은 후에도. 받은 세션에 대해서 사용하기 전에 이 녀석이 정리중인가 아닌가를 확인하는 과정이 필요하다.
	if (0 != (SESSION_DELETE_FLAG & InterlockedIncrement(&sessionPtr->m_lIoCount)))
	{
		DecreaseIoCount(sessionPtr);

		return nullptr;
	}

	return sessionPtr;
}

void jh::IocpClient::DecreaseIoCount(Session* sessionPtr)
{
	// Log
	LONG ioCount = InterlockedDecrement(&sessionPtr->m_lIoCount);

	if (0 == ioCount)
		DeleteSession(sessionPtr->m_ullSessionId);

}



jh::Session* jh::IocpClient::CreateSession(SOCKET sock, const SOCKADDR_IN* pSockAddr)
{
	// 여기부터 세션빼서 내껄로 사용하는거 적기,
	DWORD availableIndex = UINT_MAX;

	if (false == m_sessionIndexStack.TryPop(availableIndex))
	{
		_LOG(m_pcwszClientName, LOG_LEVEL_INFO, L"[CreateSession] Session pool is empty, SessionCount : [%u].", m_lSessionCount);

		return nullptr;
	}

	// 세션 수를 증가하고.
	InterlockedIncrement(&m_lSessionCount);

	Session* sessionPtr = &m_pClientSessionArr[availableIndex];

	static ULONGLONG sessionIdGen = 0;

	// Session에 등록할 id
	// [16 idx ][48 id]

	ULONGLONG id = (static_cast<ULONGLONG>(availableIndex) << SESSION_IDX_SHIFT_BIT) | ((++sessionIdGen) & SESSION_ID_MASKING_BIT);

	//sessionPtr->m_ullSessionId = id;

	// 세션 초기화
	sessionPtr->Activate(sock, pSockAddr, id);

	_LOG(m_pcwszClientName, LOG_LEVEL_DEBUG, L"[CreateSession] Session Index : 0x%016llx, Session ID : 0x%016llx ", availableIndex, id);
	// 세션 등록 실패
	HANDLE ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), m_hCompletionPort, reinterpret_cast<ULONG_PTR>(sessionPtr), 0);
	if (nullptr == ret)
	{
		m_sessionIndexStack.Push(availableIndex);

		InterlockedDecrement(&m_lSessionCount);

		_LOG(m_pcwszClientName, LOG_LEVEL_WARNING, L"[CreateSession] CreateIoCompletionPort() failed");

		return nullptr;
	}

	return sessionPtr;

}

void jh::IocpClient::DeleteSession(ULONGLONG sessionId)
{
	if (sessionId == INVALID_SESSION_ID)
	{
		_LOG(m_pcwszClientName, LOG_LEVEL_WARNING, L"[DeleteSession] Invalid Session ID: [0X%016llx]", sessionId);
		return;
	}

	LONGLONG sessionIdx = sessionId >> SESSION_IDX_SHIFT_BIT;

	Session* sessionPtr = &m_pClientSessionArr[sessionIdx];

	// 세션을 그냥 얻어가면 줬을 때 내가 준 세션이 해제되었을 수 있다. 그래서 ID 확인 후 건네주도록 한다.
	if (sessionId != sessionPtr->m_ullSessionId)
	{
		_LOG(m_pcwszClientName, LOG_LEVEL_DEBUG, L"[DeleteSession] Session ID != _SessionArray[SessionIdx].SessionID, SessionID : [0x%016llx], SessionIdx : [0x%016llx], called by [%s]", sessionId, sessionIdx, PROF_WFUNC);

		return;
	}

	if (InterlockedCompareExchange(&sessionPtr->m_lIoCount, SESSION_DELETE_FLAG, 0) != 0)
	{
		_LOG(m_pcwszClientName, LOG_LEVEL_INFO, L" [DeleteSession] Session is Using. SessionId : [0x%016llx]", sessionId);
		return;
	}
	InterlockedIncrement64(&m_llTotalDisconnectCount);

	OnDisconnected(sessionId);

	closesocket(sessionPtr->m_socket);

	sessionPtr->Clear();

	m_sessionIndexStack.Push(sessionIdx);

	InterlockedDecrement(&m_lSessionCount);

	_LOG(m_pcwszClientName, LOG_LEVEL_DEBUG, L"[DeleteSessIon] PushStack - SessionID : [0x%016llx], SessionIdx : [0x%016llx]", sessionId, sessionIdx);
	return;
}

void jh::IocpClient::InitClientConfig(WCHAR* ip, WORD port, DWORD concurrentWorkerThreadCount, WORD lingerOnOff, WORD lingerTime, ULONGLONG timeOut)
{
	wcscpy_s(m_wszTargetIp, ip);
	m_usTargetPort = port;
	m_dwConcurrentWorkerThreadCount = concurrentWorkerThreadCount;
	m_lingerOption.l_onoff = lingerOnOff;
	m_lingerOption.l_linger = lingerTime;

	m_ullTimeoutLimit = timeOut;
}

bool jh::IocpClient::InitSessionArray(DWORD maxSessionCount)
{
	if (m_pClientSessionArr != nullptr)
		return false;

	m_dwMaxSessionCnt = maxSessionCount;
	m_pClientSessionArr = new Session[m_dwMaxSessionCnt];

	m_sessionIndexStack.Reserve(m_dwMaxSessionCnt);

	for (DWORD dw = 0; dw < m_dwMaxSessionCnt; dw++)
	{
		m_sessionIndexStack.Push(dw);
	}

	return true;
}

void jh::IocpClient::ClearSessions()
{
	// 정리되지 않은 세션들을 정리
	for (int i = 0; i < m_dwMaxSessionCnt; i++)
	{
		Session* sessionPtr = &m_pClientSessionArr[i];

		if (INVALID_SOCKET == sessionPtr->m_socket)
			continue;

		LONGLONG sessionIdx = sessionPtr->m_ullSessionId >> SESSION_IDX_SHIFT_BIT;

		closesocket(sessionPtr->m_socket);
		sessionPtr->Clear();

		m_sessionIndexStack.Push(sessionIdx);

		InterlockedDecrement(&m_lSessionCount);
	}
}


jh::IocpClient::IocpClient(const WCHAR* clientName) : m_pcwszClientName(clientName), m_hCompletionPort(nullptr), m_hWorkerThreads(nullptr), m_wszTargetIp{}, m_pClientSessionArr(nullptr), m_lingerOption{}
{
	m_dwConcurrentWorkerThreadCount = 0;
	m_dwMaxSessionCnt = 0;
	m_usTargetPort = 0;
	m_ullTimeoutLimit = 0;

	//_sessionLog = new SessionLog[sessionLogMax];

	m_llDisconnectedCount = 0;
	m_llTotalDisconnectCount = 0;

	m_lSessionCount = 0;

	m_llTotalConnectedSessionCount = 0;

	m_lSendCount = 0;
	m_lRecvCount = 0;
}
jh::IocpClient::~IocpClient()
{

}

bool jh::IocpClient::Start()
{
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, m_dwConcurrentWorkerThreadCount);

	if (m_hCompletionPort == NULL)
	{
		_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L"[Start] IOCP Handle is NULL.");

		return false;
	}

	m_hWorkerThreads = new HANDLE[m_dwConcurrentWorkerThreadCount];

	for (int i = 0; i < m_dwConcurrentWorkerThreadCount; i++)
	{
		m_hWorkerThreads[i] = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, IocpClient::WorkerThreadFunc, this, 0, nullptr));

		if (nullptr == m_hWorkerThreads[i])
		{
			_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L"[Start] WorkerThread %d creation failed.", i);

			jh_utility::CrashDump::Crash();
		}
	}

	OnStart();

	return true;
}

void jh::IocpClient::Stop()
{
	OnStop();

	if (nullptr != m_hWorkerThreads)
	{
		for (int i = 0; i < m_dwConcurrentWorkerThreadCount; i++)
		{
			PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, nullptr);
		}

		DWORD ret = WaitForMultipleObjects(m_dwConcurrentWorkerThreadCount, m_hWorkerThreads, true, INFINITE);
		if (!(WAIT_OBJECT_0 <= ret && (WAIT_OBJECT_0 + m_dwConcurrentWorkerThreadCount) > ret))
		{
			DWORD gle = GetLastError();
			_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L"[Stop] WaitForMultipleObjects failed. GetLastError: [%u]", gle);
		}

		for (int i = 0; i < m_dwConcurrentWorkerThreadCount; i++)
		{
			CloseHandle(m_hWorkerThreads[i]);
		}

		delete[] m_hWorkerThreads;
		m_hWorkerThreads = nullptr;
	}

	if (nullptr != m_hCompletionPort)
	{
		CloseHandle(m_hCompletionPort);

		m_hCompletionPort = nullptr;
	}

	ClearSessions();
}

void jh::IocpClient::Connect(int cnt)
{
	SOCKADDR_IN serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_usTargetPort);
	serverAddr.sin_addr = NetAddress::IpToAddr(m_wszTargetIp);

	for (int i = 0; i < cnt; i++)
	{

		SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

		if (INVALID_SOCKET == sock)
		{
			int gle = WSAGetLastError();

			_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L"[Connect] Invalid socket. WSAGetLastError : [%d]", gle);

			closesocket(sock);
			return;
		}

		// connectEx 사용 시 bind 필요
		sockaddr_in clientAddr{ AF_INET,htonl(INADDR_ANY) , htons(0) };

		DWORD bindRet = bind(sock, (SOCKADDR*)&clientAddr, sizeof(SOCKADDR_IN));

		if (SOCKET_ERROR == bindRet)
		{
			int gle = WSAGetLastError();

			_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L"[Connect] Bind failed. GetLastError : [%d]", gle);

			closesocket(sock);

			return;
		}

		DWORD setLingerRet = setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&m_lingerOption, sizeof(linger));

		if (SOCKET_ERROR == setLingerRet)
		{
			int gle = WSAGetLastError();

			_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L"[Connect] SetLingerOpt failed. GetLastError: [%d]", gle);

			closesocket(sock);
			return;
		}

		Session* session = CreateSession(sock, &serverAddr);

		if (nullptr == session)
		{
			closesocket(sock);

			return;
		}

		//	connect에 대한 ioCount 따로 등록
		// CreateSession에서 처음에 등록해놓는다.
		//InterlockedIncrement(&session->m_lIoCount);

		DWORD bytes = 99;

		bool retConnectEx = jh::NetAddress::lpfnConnectEx(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr), nullptr, 0, &bytes, &session->m_connectOverlapped);

		if (false == retConnectEx)
		{
			int wsaGetLastError = WSAGetLastError();

			// 서버가 켜져있지 않아도 wsa_io_pending이 나옴. 완료통지 결과를 확인해야한다.
			if (WSA_IO_PENDING != wsaGetLastError)
			{
				_LOG(m_pcwszClientName, LOG_LEVEL_SYSTEM, L"[Connect] ConnectEx failed. Session ID: [0x%016llx]", session->m_ullSessionId);

				DecreaseIoCount(session);
			}
		}
	}
}

