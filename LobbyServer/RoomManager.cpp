#include "pch.h"
#include "RoomManager.h"
#include "NetworkBase.h"
#include "User.h"
#include "Memory.h"
#include "Room.h"

jh::RoomManager::RoomManager(USHORT maxRoomCnt, USHORT maxRoomUserCnt, const Room::UnicastFunc& unicastFunc, const Room::OnGameStartFunc& onGameStartFunc) : m_usMaxRoomCnt{ maxRoomCnt }, m_usMaxRoomUserCnt{ maxRoomUserCnt }, m_unicastFunc{ unicastFunc },
m_onGameStartFunc{ onGameStartFunc }, m_usUsingRoomCnt{}
{
	InitializeSRWLock(&m_lock);
	m_roomMap.reserve(m_usMaxRoomCnt);
}

jh::RoomManager::~RoomManager()
{
}

RoomRef jh::RoomManager::GetRoom(USHORT roomNum)
{
	SRWSharedLockGuard sharedLockGuard(&m_lock);

	auto iter = m_roomMap.find(roomNum);
	if (iter == m_roomMap.end())
	{
		_LOG(ROOM_MANAGER_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[GetRoom] Room not found. Room : [%hu].", roomNum);
		return nullptr;
	}

	return iter->second;
}

std::vector<jh::RoomInfo> jh::RoomManager::GetRoomInfoList()
{
	PRO_START_AUTO_FUNC;

	std::vector<std::weak_ptr<Room>> v;

	{	
		SRWSharedLockGuard srwSharedLock(&m_lock);

		v.reserve(m_roomMap.size());
		
		for (const auto& [roomNum, room] : m_roomMap)
		{
			v.push_back(room);
		}
	}

	std::vector<jh::RoomInfo> infoList;

	infoList.reserve(v.size());

	for (const auto& weak : v)
	{
		RoomRef roomRef = weak.lock();

		if (nullptr == roomRef)
			continue;

		infoList.push_back(roomRef->GetRoomInfo());		
	}

	return infoList;
}

RoomRef jh::RoomManager::CreateRoom(UserRef user, WCHAR* roomName)
{
	PRO_START_AUTO_FUNC;

	RoomInfo roomInfo;

	roomInfo.m_ullOwnerId = user->GetUserId();
	roomInfo.m_usCurUserCnt = 1;
	roomInfo.m_usMaxUserCnt = m_usMaxRoomUserCnt;
	wcscpy_s(roomInfo.m_wszRoomName, roomName);
	
	{
		USHORT usingRoomCount = static_cast<USHORT>(InterlockedIncrement16(&m_usUsingRoomCnt));

		// 이미 100개가 존재하는 상태
		if (usingRoomCount > m_usMaxRoomCnt)
		{
			InterlockedDecrement16(&m_usUsingRoomCnt);

			return nullptr;
		}
	}

	static SHORT roomNumGen = 0;


	USHORT roomNum = InterlockedIncrement16(&roomNumGen) % USHRT_MAX;
	
	roomInfo.m_usRoomNum = roomNum;

	auto onEmptyRoomFunc = [this](USHORT roomNum)
		{
			DestroyRoom(roomNum);
		};

	RoomRef room = jh::MakeShared<jh::Room>(roomInfo, m_unicastFunc, onEmptyRoomFunc, m_onGameStartFunc);

	room->DoAsync([room, user]()
		{
			room->OnCreated(user);
		});

	SRWLockGuard lockGuard(&m_lock);
		m_roomMap.insert({ roomNum, room });

	_LOG(L"RoomManager", LOG_LEVEL_INFO, L"[CreateRoom] Roomnum: [%hu]", roomNum);

	return room;
}

void jh::RoomManager::DestroyRoom(USHORT roomNum)
{
	PRO_START_AUTO_FUNC;

	size_t delSize;

	{
		SRWLockGuard lockGuard(&m_lock);

		delSize = m_roomMap.erase(roomNum);
	}
	
	if (0 == delSize)
	{
		_LOG(L"RoomManager", LOG_LEVEL_WARNING, L"[DestroyRoom] Room not found: [%hu]", roomNum);

		return;
	}

	InterlockedDecrement16(&m_usUsingRoomCnt);

}