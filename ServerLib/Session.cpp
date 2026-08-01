#include "LibraryPch.h"
#include "Session.h"
#include "NetworkBase.h"

using namespace jh_utility;

/*------------------------------
			Session
------------------------------*/

jh::Session::Session()
{
	Clear();

#ifdef USE_SESSION_LOG
	m_pSessionLogger = jh::MakeUnique<SessionLoggger>();
#endif
}

jh::Session::~Session()
{
}


void jh::Session::Clear()
{
	m_ullSessionId = INVALID_SESSION_ID;

	m_socket = INVALID_SOCKET;
	m_targetNetAddr.Reset();

	m_ullLastTimeStamp = 0; // jh_utility::GetTimeStamp();

	m_sendQ.Clear();

	m_recvBuffer.ClearBuffer();
	m_recvOverlapped.Reset();
	m_sendOverlapped.Reset();
	m_connectOverlapped.Reset();

	InterlockedExchange(&m_lIoCount, 0);
	InterlockedExchange8(&m_bSendFlag, 1);
	InterlockedExchange8(&m_bConnectedFlag, 0);
}

bool jh::Session::Activate(SOCKET sock, const SOCKADDR_IN* sockAddr, ULONGLONG newId)
{
	// 세션을 할당할때 사용가능한 상태로 초기화
	if (InterlockedCompareExchange(&m_lIoCount, 1, 0) != 0)
		return false;

	m_ullSessionId = newId;
	
	m_socket = sock;
	m_targetNetAddr = *sockAddr;

	m_ullLastTimeStamp = jh_utility::GetTimeStamp();

	InterlockedExchange8(&m_bSendFlag, 0);
	//InterlockedExchange(&m_lIoCount, 0);
	InterlockedExchange8(&m_bConnectedFlag, 1);
	
	return true;
}



jh::ActiveSessionManager::ActiveSessionManager()
{
	InitializeSRWLock(&m_lock);
}

jh::ActiveSessionManager::~ActiveSessionManager()
{
}

void jh::ActiveSessionManager::Reserve(size_t reserveSize)
{
	SRWLockGuard lockGuard(&m_lock);

	m_activeSessionMap.reserve(reserveSize);
}

void jh::ActiveSessionManager::AddActiveSession(Session* sessionPtr)
{
	SRWLockGuard lockGuard(&m_lock);
	
	ULONGLONG sessionIdWithIdx = sessionPtr->m_ullSessionId;
	
	m_activeSessionMap.insert(std::make_pair(sessionIdWithIdx, sessionPtr));
}

void jh::ActiveSessionManager::RemoveActiveSession(ULONGLONG sessionIdWithIdx)
{
	SRWLockGuard lockGuard(&m_lock);

	m_activeSessionMap.erase(sessionIdWithIdx);
}

std::vector<ULONGLONG> jh::ActiveSessionManager::GetSessionSnapshot()
{
	std::vector<ULONGLONG> snapshot;

	// m_activeSessinoMap의 size는 과거 값을 읽어도 상관없음. 
	size_t reserveSize = m_activeSessionMap.size();

	// 여유분을 추가하여 버퍼 예약
	snapshot.reserve(reserveSize + 5);

	SRWLockGuard lockGuard(&m_lock);

	// sessionid / sessionPtr
	for (const auto& sessionInfo : m_activeSessionMap)
	{
		snapshot.push_back(sessionInfo.first);
	}

	return snapshot;
}

