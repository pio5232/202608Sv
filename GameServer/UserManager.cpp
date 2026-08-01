#include "pch.h"
#include "GamePlayer.h"
#include "UserManager.h"
#include "GameServer.h"
#include "AIPlayer.h"
#include "GameWorld.h"
#include "Memory.h"
#include "User.h"
using namespace jh;

UserRef jh::UserManager::CreateUser(ULONGLONG sessionId, ULONGLONG userId)
{
	auto sessionIt = m_sessionIdToUserUMap.find(sessionId);
	auto userIt = m_userIdToUserUMap.find(userId);

	if (sessionIt != m_sessionIdToUserUMap.end())
	{
		_LOG(GAME_USER_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_SYSTEM, L"[CreateUser] Duplicate session. SessionId : [0x%016llx]", sessionId);
		return nullptr;
	}

	if (userIt != m_userIdToUserUMap.end())
	{
		_LOG(GAME_USER_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_SYSTEM, L"[CreateUser] Duplicate user. UserId : [%llu]", userId);
		return nullptr;
	}

	UserRef user = jh::MakeShared<jh::User>(sessionId, userId);
	
	m_sessionIdToUserUMap.insert({ sessionId, user });
	m_userIdToUserUMap.insert({ userId, user });
	
	return user;
}



void jh::UserManager::RemoveUser(ULONGLONG sessionId)
{
	UserRef user = GetUserBySessionId(sessionId);

	if (nullptr == user)
	{
		_LOG(GAME_USER_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[RemoveUser] User not found. SessionId: [0x%016llx]", sessionId);
		return;
	}

	ULONGLONG userId = user->GetUserId();

	m_sessionIdToUserUMap.erase(sessionId);
	m_userIdToUserUMap.erase(userId);
}

void jh::UserManager::UnicastFunc(ULONGLONG sessionId, PacketBufferRef& packet)
{
	m_sendPacketFunc(sessionId, packet);
}

void jh::UserManager::Broadcast(PacketBufferRef& packet)
{
	for (const auto& [sessionId, user] : m_sessionIdToUserUMap)
	{
		m_sendPacketFunc(sessionId, packet);
	}
}


void jh::UserManager::RegisterEntityIdToUser(ULONGLONG entityId, UserRef user)
{
	if (m_entityIdToUserUMap.end() != m_entityIdToUserUMap.find(entityId))
	{
		_LOG(GAME_USER_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[RegisterEntityIdToUser] Duplicated entity registration. EntityId : [%llu]", entityId);

		return;
	}

	m_entityIdToUserUMap.insert({ entityId, user });
}

void jh::UserManager::DeleteEntityIdToUser(ULONGLONG entityId)
{
	m_entityIdToUserUMap.erase(entityId);
}

UserRef jh::UserManager::GetUserByUserId(ULONGLONG userId)
{
	std::unordered_map<ULONGLONG, UserRef>::iterator iter = m_userIdToUserUMap.find(userId);

	if (iter != m_userIdToUserUMap.end())
		return iter->second;

	_LOG(GAME_USER_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[GetUserByUserId] User not found. UserId : [%llu]", userId);

	return nullptr;
}

UserRef jh::UserManager::GetUserBySessionId(ULONGLONG sessionId)
{
	std::unordered_map<ULONGLONG, UserRef>::iterator iter = m_sessionIdToUserUMap.find(sessionId);

	if (iter != m_sessionIdToUserUMap.end())
		return iter->second;

	_LOG(GAME_USER_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[GetUserBySessionId] User not found. SessionId : [0x%016llx]", sessionId);

	return nullptr;
}

UserRef jh::UserManager::GetUserByEntityId(ULONGLONG entityId)
{
	std::unordered_map<ULONGLONG, UserRef>::iterator iter = m_entityIdToUserUMap.find(entityId);

	if (iter != m_entityIdToUserUMap.end())
		return iter->second;

	_LOG(GAME_USER_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[GetUserByEntityId] User not found. EntityId : [%llu]", entityId);
	return nullptr;
}

void jh::UserManager::ReserveUMapSize(USHORT requiredUsers, USHORT maxUsers)
{
	m_sessionIdToUserUMap.reserve(requiredUsers);
	m_userIdToUserUMap.reserve(requiredUsers);
}