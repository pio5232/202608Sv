#pragma once
#include "Session.h"
//#include "PacketDefine.h"

namespace jh
{
	/*-----------------------
			IocpServer
	-----------------------*/

	class IocpServer
	{
	public:
		IocpServer(const WCHAR* serverName);
		virtual ~IocpServer() = 0;

		bool Start();
		void Stop();

		void ClearSessions();

		// 각각의 함수들은 Start() / Stop()가 실행됐을 때
		// 상속받은 함수에서 추가적으로 작업할 것들을 여기에 등록하면 된다.
		virtual void OnStarted() = 0;
		virtual void OnStop() = 0;

		virtual bool OnConnectionRequest(const SOCKADDR_IN& clientInfo) = 0;
		virtual void OnError(int errCode, WCHAR* cause) = 0;

		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef dataBuffer, USHORT type) = 0;

		virtual void OnConnected(ULONGLONG sessionId) = 0;
		virtual void OnDisconnected(ULONGLONG sessionId) = 0;

		void ProcessRecv(Session* sessionPtr, DWORD transferredBytes);
		void ProcessSend(Session* sessionPtr, DWORD transferredBytes);

		void Disconnect(ULONGLONG sessionId, const WCHAR* reason);

		// WSASEND / WSARECV를 등록
		void PostSend(Session* sessionPtr);
		void PostRecv(Session* sessionPtr);

		void SendPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		
		const WCHAR* const GetServerName() const { return m_pcwszServerName; }

		LONG GetSessionCount() const { return m_lSessionCount; }

		const WCHAR* GetIp() const { return m_pConfig->m_wszIp; }
		USHORT GetPort() const;

		bool InitSessionArray(DWORD maxSessionCount);

		void SendRequest(Session* sessionPtr, PacketBufferRef& packet);

#ifdef USE_SESSION_LOG
		void PostSend(Session* sessionPtr, DWORD entry);
#endif
	protected:
		void CheckHeartbeatTimeout();
		void UpdateHeartbeat(ULONGLONG sessionId, ULONGLONG now);

		LONG GetRecvCount() { return InterlockedExchange(&m_lRecvCount, 0); }
		LONG GetSendCount() { return InterlockedExchange(&m_lSendCount, 0); }

		LONGLONG GetTotalAcceptedCount() const { return m_llTotalAcceptedSessionCount; }
		LONGLONG GetDisconnectedCount() const { return m_llDisconnectedCount; }
		LONGLONG GetTotalDisconnectedCount() const { return m_llTotalDisconnectCount; }

		SOCKET GetListenSock() const { return m_listenSock; }

		const ServerConfig* GetConfig();
		virtual ServerConfig* CreateConfig() = 0;
	private:

		Session* TryAcquireSession(ULONGLONG sessionId, const WCHAR* caller); // sessionPtr을 사용할 때마다 해제중인지 확인하고, 참조 카운트를 증가시킨다.

		void InitializeServerTasks();
		virtual void OnInitialized() = 0;

		void DeleteSession(ULONGLONG sessionId);
		void DecreaseIoCount(Session* sessionPtr);

		bool ProcessIO(DWORD timeout = INFINITE);
		void ProcessAccept();

		Session* CreateSession(SOCKET sock, const SOCKADDR_IN* pSockAddr);
		HANDLE							m_hCompletionPort;

	private:

		// 파생 클래스에서 해야할 작업을 정의하도록 한다.
		virtual void OnWorkerThreadUpdateBegin() = 0;
		virtual void OnWorkerThreadUpdateEnd() = 0;

		ServerConfig*					m_pConfig;								// 설정 값, 초기값은 nullptr, Start 시 파생 클래스에서 생성
		const WCHAR* const				m_pcwszServerName;						// 실행중인 서버의 이름 // Chatting / Lobby / Game.. 등등
		SOCKET							m_listenSock;

		jh::Session*					m_pSessionArray;
		jh_utility::LockStack<DWORD>	m_sessionIndexStack;
		
		ActiveSessionManager			m_activeSessionManager;
		jh_utility::ThreadExecutor		m_workerExecutor;

		alignas(64) LONG				m_lSessionCount;						// 현재 접속중인 세션 수
		alignas(64) LONGLONG			m_llTotalAcceptedSessionCount;			// 시작부터 연결된 세션의 개수 

		alignas(64) LONGLONG			m_llDisconnectedCount;					// 상대쪽에서 연결을 끊은 횟수

		alignas(64) LONGLONG			m_llTotalDisconnectCount;				// 상대 + 서버가 연결을 끊은 횟수

		alignas(64) LONG				m_lRecvCount;							// 1초동안 Recv처리된 패킷의 수
		alignas(64) LONG				m_lSendCount;							// 1초동안 Send완료된 패킷의 수

	};
}