#include "pch.h"
#include "UserManager.h"
#include "User.h"

/// +---------------------------+
/// |		UserManager			|
/// +---------------------------+

jh::UserManager::UserManager()
{
	InitializeSRWLock(&m_lock);
}

jh::UserManager::~UserManager()
{
}

void jh::UserManager::Init()
{
}

UserRef jh::UserManager::CreateUser(ULONGLONG sessionId)
{
	PRO_START_AUTO_FUNC;

	SRWLockGuard lockGuard(&m_lock);

	if (m_sessionIdToUserUMap.find(sessionId) != m_sessionIdToUserUMap.end())
		return nullptr;

	static ULONGLONG newUserId = 4283;
	newUserId += 3;

	UserRef user = std::make_shared<jh::User>(sessionId, newUserId);

	m_sessionIdToUserUMap[sessionId] = user;
	m_userIdToUserUMap[newUserId] = user;
	
	return user;
}

void jh::UserManager::RemoveUser(ULONGLONG sessionId)
{
	PRO_START_AUTO_FUNC;

	SRWLockGuard lockGuard(&m_lock);

	auto findIt = m_sessionIdToUserUMap.find(sessionId);

	if (m_sessionIdToUserUMap.end() == findIt)
	{
		_LOG(L"UserManager", LOG_LEVEL_WARNING, L"[RemoveUser] User not found. SessionId: [0x%016llx]", sessionId);
		return;
	}

	ULONGLONG userId = findIt->second->GetUserId();

	m_userIdToUserUMap.erase(userId);
	m_sessionIdToUserUMap.erase(sessionId);
}

UserRef jh::UserManager::GetUserByUserId(ULONGLONG userId)
{
	PRO_START_AUTO_FUNC;

	SRWSharedLockGuard sharedLockGuard(&m_lock);

	auto userIter = m_userIdToUserUMap.find(userId);

	if (m_userIdToUserUMap.end() == userIter)
	{
		_LOG(L"UserManager", LOG_LEVEL_WARNING, L"[GetUserByUserId] User not found. UserId: [%llu]", userId);
		return nullptr;
	}
	
	return m_userIdToUserUMap[userId];
}

UserRef jh::UserManager::GetUserBySessionId(ULONGLONG sessionId)
{
	PRO_START_AUTO_FUNC;

	SRWSharedLockGuard sharedLockGuard(&m_lock);

	auto userIter = m_sessionIdToUserUMap.find(sessionId);

	// 세션만 연결된 경우에는 유저가 존재하지 않음.
	if (m_sessionIdToUserUMap.end() == userIter)
	{
		_LOG(L"UserManager", LOG_LEVEL_INFO, L"[GetUserBySessionId] User not found. SessionId: [0x%016llx]", sessionId);	
		return nullptr;
	}

	return m_sessionIdToUserUMap[sessionId];
}