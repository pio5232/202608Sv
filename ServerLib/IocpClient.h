#pragma once

namespace jh {
	class IocpClient
	{
	public:
		IocpClient(const WCHAR* clientName);
		virtual ~IocpClient() = 0;

		bool Start();
		void Stop();

		void Connect(int cnt);
		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef dataBuffer, USHORT type) = 0;

		virtual void OnConnected(ULONGLONG sessionId) = 0;
		virtual void OnDisconnected(ULONGLONG sessionId) = 0;

		void ProcessRecv(Session* sessionPtr, DWORD transferredBytes);
		void ProcessSend(Session* sessionPtr, DWORD transferredBytes);

		void Disconnect(ULONGLONG sessionId);

		void PostSend(Session* sessionPtr);
		void PostRecv(Session* sessionPtr);

		void SendPacket(ULONGLONG sessionId, PacketBufferRef& packet);

		static unsigned WINAPI WorkerThreadFunc(LPVOID lparam);

		void WorkerThreadMain();

		Session* CreateSession(SOCKET sock, const SOCKADDR_IN* pSockAddr);

		void InitClientConfig(WCHAR* ip, WORD port, DWORD concurrentWorkerThreadCount, WORD lingerOnOff, WORD lingerTime, ULONGLONG timeOut);

		bool InitSessionArray(DWORD maxSessionCount);

		void ClearSessions(); // 강제 종료

		int GetMaxSessionCount() const { return m_dwMaxSessionCnt; }
		LONG GetSessionCount() const { return m_lSessionCount; }
		LONG GetRecvCount() { return InterlockedExchange(&m_lRecvCount, 0); }
		LONG GetSendCount() { return InterlockedExchange(&m_lSendCount, 0); }

		LONGLONG GetTotalConnectedCount() const { return m_llTotalConnectedSessionCount; }
		LONGLONG GetDisconnectedCount() const { return m_llDisconnectedCount; }
		LONGLONG GetTotalDisconnectedCount() const { return m_llTotalDisconnectCount; }
	protected:
		virtual void OnStart() {}
		virtual void OnStop() {}
		const WCHAR* const m_pcwszClientName;

		// 설정 값
		WCHAR m_wszTargetIp[IP_STRING_LEN];								// ip
		USHORT m_usTargetPort;											// 포트 번호
		DWORD m_dwMaxSessionCnt;										// 한번에 접속가능한 최대 세션 수
		DWORD m_dwConcurrentWorkerThreadCount;							// iocp에 등록할 worker 수
		LINGER m_lingerOption;											// TIME_OUT 옵션 설정
		ULONGLONG m_ullTimeoutLimit;									// HEARTBEAT

	private:
		Session* TryAcquireSession(ULONGLONG sessionId);				// sessionPtr을 사용할 때마다 해제중인지 확인하고, 참조 카운트를 증가시킨다.

		void DecreaseIoCount(Session* sessionPtr);
		void DeleteSession(ULONGLONG sessionId);

		Session* m_pClientSessionArr;
		jh_utility::LockStack<DWORD> m_sessionIndexStack;

		const NetAddress m_targetNetAddr;

		HANDLE m_hCompletionPort;
		HANDLE* m_hWorkerThreads;

		alignas(64) LONG m_lSessionCount;							// 현재 연결된 Session의 수.
		alignas(64) LONGLONG m_llTotalConnectedSessionCount;		// 시작부터 연결된 세션의 총 합

		alignas(64) LONG m_llDisconnectedCount;						// 상대쪽에서 연결을 끊은 횟수
		alignas(64) LONGLONG m_llTotalDisconnectCount;			// 상대 + 본인이 연결을 끊은 횟수

		alignas(64) LONG m_lRecvCount;							// 1초동안 Recv처리된 패킷의 수
		alignas(64) LONG m_lSendCount;							// 1초동안 Send완료된 패킷의 수

	};


}
