#pragma once

#include <queue>
#include <stack>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <set>

//#define USE_SESSION_LOG

#define INVALID_SESSION_ID (LONGLONG)(~0)
#define INVALID_SESSION_INDEX (LONGLONG)(~0)

#define SESSION_IDX_SHIFT_BIT 48
#define SESSION_IDX_SHIFT_MASKING_BIT (0xffff'0000'0000'0000)

#define SESSION_ID_MASKING_BIT (0x0000'ffff'ffff'ffff)
#define SESSION_DELETE_FLAG 0x8000'0000

enum class OverlappedType
{
	RECV,
	SEND,
	CONNECT,
};

struct CustomOverlapped : public OVERLAPPED
{
	CustomOverlapped(OverlappedType overlappedType) : OVERLAPPED{}, m_eOverLappedType { overlappedType }
	{

	}

	const OverlappedType m_eOverLappedType;

};
struct RecvOverlapped : public CustomOverlapped
{
	RecvOverlapped() : CustomOverlapped(OverlappedType::RECV){}
	
	// 가상 함수로 만들면 안됨!!
	void Reset() 
	{
		memset(this, 0, sizeof(OVERLAPPED));
	}
};

struct SendOverlapped : public CustomOverlapped
{
	SendOverlapped() : CustomOverlapped(OverlappedType::SEND)
	{
	}

	void Reset()
	{
		ClearPendingList();

		memset(this, 0, sizeof(OVERLAPPED));
	}

	void ClearPendingList()
	{
		 m_pendingList.clear();
	}

	std::vector<PacketBufferRef> m_pendingList;
};

struct ConnectOverlapped : public CustomOverlapped
{
	ConnectOverlapped() : CustomOverlapped(OverlappedType::CONNECT) {}

	void Reset()
	{
		memset(this, 0, sizeof(OVERLAPPED));
	}

};

namespace jh
{
#ifdef USE_SESSION_LOG
	enum SendLogEntry : DWORD
	{
		PROCESS_SEND = 1,	// SEND 완료통지를 통해 후처리도중에 진입
		SEND_PACKET = 2,	// 컨텐츠에서 SEND (OnRecv를 통해 처리되는 컨텐츠코드)를 통해 진입
	};

	enum RetReason : DWORD
	{
		SEND_FLAG_1 = 1,	// SEND_FLAG가 이미 설정되어 있다.
		USE_SIZE_0,			// SEND_FLAG를 바꾸고 진입했지만 USE_SIZE가 0이다.
		CONNECTED_FALSE,		// IO 작업을 막는 변수가 활성화되어있다.
	};

	class SessionLoggger
	{
	public:
		void Log(DWORD entry, DWORD reason, DWORD entryOrder)
		{
			LONG order = InterlockedIncrement(&m_lOrderGen);
			LONG logIdx = order % kLogCount;

			m_log[logIdx].m_lOrder = order;
			m_log[logIdx].m_dwEntry = entry;
			m_log[logIdx].m_dwEntryOrder = entryOrder;
			m_log[logIdx].m_dwReason = reason;
			m_log[logIdx].m_dwThreadId = GetCurrentThreadId();
		}

		LONG GetSnapShot() { return InterlockedIncrement(&m_lOrderSnapshot); }
	private:
		struct SessionLog
		{
			LONG	m_lOrder{};			// 번호
			DWORD	m_dwEntry{};		// 어느 함수에서 호출되었는가.
			LONG	m_dwEntryOrder{};	// 언제 호출되었는가.
			DWORD	m_dwReason{};		// 리턴한 이유는 무엇인가.
			DWORD	m_dwThreadId{};		// 어느 스레드에서 실행되었는가.
		};

		static const int	kLogCount = 200;

		SessionLog			m_log[kLogCount]{};

		alignas(64) LONG	m_lOrderGen {};
		alignas(64) LONG	m_lOrderSnapshot {}; // 별도의 순서 캡쳐를 위해 사용
	};
#endif
	/*------------------------------
				Session 
	------------------------------*/

	// [ 4 (arrayIndex) / 4 (id) ]
	struct Session 
	{
	public:
		Session();
		~Session();

		void Clear();
		bool Activate(SOCKET sock, const SOCKADDR_IN* sockAddr, ULONGLONG newId);

	public:
		LONGLONG								m_ullSessionId;
		SOCKET									m_socket;
		NetAddress								m_targetNetAddr;

		ULONGLONG								m_ullLastTimeStamp;
	
		jh_utility::LockQueue<PacketBufferRef>	m_sendQ;
		jh_utility::RingBuffer					m_recvBuffer;

		RecvOverlapped							m_recvOverlapped;
		SendOverlapped							m_sendOverlapped;
		ConnectOverlapped						m_connectOverlapped; // 클라이언트

		alignas(64) LONG						m_lIoCount;
		alignas(64) char						m_bSendFlag; // Use - 1, unUse - 0
		alignas(64) char						m_bConnectedFlag;

#ifdef USE_SESSION_LOG
		jh::UniquePtr<SessionLoggger>			m_pSessionLogger;
#endif
		alignas(64) ULONGLONG data;
	};

	/*------------------------------------------
				ActiveSessionManager
	------------------------------------------*/

	class ActiveSessionManager
	{
	public:
		ActiveSessionManager(); 
		~ActiveSessionManager();

		void Reserve(size_t reserveSize);
		void AddActiveSession(Session* sessionPtr);
		void RemoveActiveSession(ULONGLONG sessionIdWithIdx); // [index(16) + SessionId]

		std::vector<ULONGLONG> GetSessionSnapshot();

	private:
		std::unordered_map<ULONGLONG, Session*> m_activeSessionMap;
		SRWLOCK									m_lock;
	};
}
