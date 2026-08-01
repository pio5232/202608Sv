#pragma once
//#include <unordered_map>
//#include <iostream>
//#include <thread>
//#include "Session.h"
//#include "PacketDefine.h"
//#include "CMonitor.h"
//
//#define SEND_REQUEST ((LPOVERLAPPED)0x90abcdef)
//
//namespace jh
//{
//	/*-----------------------
//			IocpServer
//	-----------------------*/
//
//	class IocpServer
//	{
//	public:
//		IocpServer(const WCHAR* serverName);
//		virtual ~IocpServer() = 0;
//
//		bool Start();
//		void Stop();
//
//		void ClearSessions();
//
//		// 각각의 함수들은 Start() / Stop()가 실행됐을 때
//		// 상속받은 함수에서 추가적으로 작업할 것들을 여기에 등록하면 된다.
//		virtual void OnStart() {};
//		virtual void OnStop() {};
//
//		virtual bool OnConnectionRequest(const SOCKADDR_IN& clientInfo);
//		virtual void OnError(int errCode, WCHAR* cause);
//
//		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef dataBuffer, USHORT type) = 0;
//
//		virtual void OnConnected(ULONGLONG sessionId) = 0;
//		virtual void OnDisconnected(ULONGLONG sessionId) = 0;
//
//		void ProcessRecv(Session* sessionPtr, DWORD transferredBytes);
//		void ProcessSend(Session* sessionPtr, DWORD transferredBytes);
//
//		void Disconnect(ULONGLONG sessionId, const WCHAR* reason);
//
//		// WSASEND / WSARECV를 등록
//		void PostSend(Session* sessionPtr);
//		void PostRecv(Session* sessionPtr);
//
//		void SendPacket(ULONGLONG sessionId, PacketBufferRef& packet);
//
//		void UpdateHeartbeat(ULONGLONG sessionId, ULONGLONG now);
//		void CheckHeartbeatTimeout();
//
//		const WCHAR* const GetServerName() const { return m_pcwszServerName; }
//
//		LONG GetSessionCount() const { return m_lSessionCount; }
//
//		const WCHAR* GetIp() const { return m_config.m_wszIp; }
//		USHORT GetPort() const;
//
//		bool InitSessionArray(DWORD maxSessionCount);
//
//		void SendRequest(Session* sessionPtr, PacketBufferRef& packet);
//
//#ifdef USE_SESSION_LOG
//		void PostSend(Session* sessionPtr, DWORD entry);
//#endif
//	protected:
//
//		LONG GetRecvCount() { return InterlockedExchange(&m_lRecvCount, 0); }
//		LONG GetSendCount() { return InterlockedExchange(&m_lSendCount, 0); }
//
//		LONGLONG GetTotalAcceptedCount() const { return m_llTotalAcceptedSessionCount; }
//		LONGLONG GetDisconnectedCount() const { return m_llDisconnectedCount; }
//		LONGLONG GetTotalDisconnectedCount() const { return m_llTotalDisconnectCount; }
//
//		SOCKET GetListenSock() const { return m_listenSock; }
//
//		void LoadConfig(const ServerConfig& baseServerConfig);
//
//
//	private:
//		Session* TryAcquireSession(ULONGLONG sessionId, const WCHAR* caller); // sessionPtr을 사용할 때마다 해제중인지 확인하고, 참조 카운트를 증가시킨다.
//
//		bool InitializeServerTasks();
//
//		void OnHeartbeatTimer();
//
//		void DeleteSession(ULONGLONG sessionId);
//		void DecreaseIoCount(Session* sessionPtr);
//
//		bool ProcessIO(DWORD timeout = INFINITE);
//		void ProcessAccept();
//
//		Session* CreateSession(SOCKET sock, const SOCKADDR_IN* pSockAddr);
//		HANDLE							m_hCompletionPort;
//
//	private:
//		ServerConfig					m_config;								// 설정 값
//		const WCHAR* const				m_pcwszServerName;						// 실행중인 서버의 이름 // Chatting / Lobby / Game.. 등등
//		SOCKET							m_listenSock;
//
//		jh::Session* m_pSessionArray;
//		jh_utility::LockStack<DWORD>	m_sessionIndexStack;
//		ActiveSessionManager			m_activeSessionManager;
//		jh_utility::ThreadExecutor		m_workerExecutor;
//		JobQueueRef						m_pJobQueue;							// 하트비트 체크 타이머를 위한 잡큐
//
//		alignas(64) LONG				m_lSessionCount;						// 현재 접속중인 세션 수
//		alignas(64) LONGLONG			m_llTotalAcceptedSessionCount;			// 시작부터 연결된 세션의 개수 
//
//		alignas(64) LONGLONG			m_llDisconnectedCount;					// 상대쪽에서 연결을 끊은 횟수
//
//		alignas(64) LONGLONG			m_llTotalDisconnectCount;				// 상대 + 서버가 연결을 끊은 횟수
//
//		alignas(64) LONG				m_lRecvCount;							// 1초동안 Recv처리된 패킷의 수
//		alignas(64) LONG				m_lSendCount;							// 1초동안 Send완료된 패킷의 수
//
//	};
//
//	class IocpClient
//	{
//	public:
//		IocpClient(const WCHAR* clientName);
//		virtual ~IocpClient() = 0;
//
//		bool Start();
//		void Stop();
//	
//		void Connect(int cnt);
//		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef dataBuffer, USHORT type) = 0;
//
//		virtual void OnConnected(ULONGLONG sessionId) = 0;
//		virtual void OnDisconnected(ULONGLONG sessionId) = 0;
//
//		void ProcessRecv(Session* sessionPtr, DWORD transferredBytes);
//		void ProcessSend(Session* sessionPtr, DWORD transferredBytes);
//
//		void Disconnect(ULONGLONG sessionId);
//
//		void PostSend(Session* sessionPtr);
//		void PostRecv(Session* sessionPtr);
//
//		void SendPacket(ULONGLONG sessionId, PacketBufferRef& packet);
//
//		static unsigned WINAPI WorkerThreadFunc(LPVOID lparam);
//
//		void WorkerThreadMain();
//
//		Session* CreateSession(SOCKET sock, const SOCKADDR_IN* pSockAddr);
//		
//		void InitClientConfig(WCHAR* ip, WORD port, DWORD concurrentWorkerThreadCount, WORD lingerOnOff, WORD lingerTime, ULONGLONG timeOut);
//		
//		bool InitSessionArray(DWORD maxSessionCount);
//
//		void ClearSessions(); // 강제 종료
//		
//		int GetMaxSessionCount() const { return m_dwMaxSessionCnt; }
//		LONG GetSessionCount() const { return m_lSessionCount; }
//		LONG GetRecvCount() { return InterlockedExchange(&m_lRecvCount, 0); }
//		LONG GetSendCount() { return InterlockedExchange(&m_lSendCount, 0); }
//
//		LONGLONG GetTotalConnectedCount() const { return m_llTotalConnectedSessionCount; }
//		LONGLONG GetDisconnectedCount() const { return m_llDisconnectedCount; }
//		LONGLONG GetTotalDisconnectedCount() const { return m_llTotalDisconnectCount; }
//	protected:
//		virtual void OnStart() {}
//		virtual void OnStop() {}
//		const WCHAR* const m_pcwszClientName;
//	
//		// 설정 값
//		WCHAR m_wszTargetIp[IP_STRING_LEN];								// ip
//		USHORT m_usTargetPort;											// 포트 번호
//		DWORD m_dwMaxSessionCnt;										// 한번에 접속가능한 최대 세션 수
//		DWORD m_dwConcurrentWorkerThreadCount;							// iocp에 등록할 worker 수
//		LINGER m_lingerOption;											// TIME_OUT 옵션 설정
//		ULONGLONG m_ullTimeoutLimit;									// HEARTBEAT
//	
//	private:
//		Session* TryAcquireSession(ULONGLONG sessionId);				// sessionPtr을 사용할 때마다 해제중인지 확인하고, 참조 카운트를 증가시킨다.
//		
//		void DecreaseIoCount(Session* sessionPtr);
//		void DeleteSession(ULONGLONG sessionId);
//
//		Session* m_pClientSessionArr;
//		jh_utility::LockStack<DWORD> m_sessionIndexStack;
//
//		const NetAddress m_targetNetAddr;
//
//		HANDLE m_hCompletionPort;
//		HANDLE* m_hWorkerThreads;
//
//		alignas(64) LONG m_lSessionCount;							// 현재 연결된 Session의 수.
//		alignas(64) LONGLONG m_llTotalConnectedSessionCount;		// 시작부터 연결된 세션의 총 합
//
//		alignas(64) LONG m_llDisconnectedCount;						// 상대쪽에서 연결을 끊은 횟수
//		alignas(64) LONGLONG m_llTotalDisconnectCount;			// 상대 + 본인이 연결을 끊은 횟수
//
//		alignas(64) LONG m_lRecvCount;							// 1초동안 Recv처리된 패킷의 수
//		alignas(64) LONG m_lSendCount;							// 1초동안 Send완료된 패킷의 수
//
//	};
//
//	
//}