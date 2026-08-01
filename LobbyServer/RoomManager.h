#pragma once

#include "Room.h"
#include "CMonitor.h"
#include "LobbyDefine.h"

namespace jh
{
	class RoomManager
	{
	public:
		RoomManager(USHORT maxRoomCnt, USHORT maxRoomUserCnt,const Room::UnicastFunc& unicastFunc, const Room::OnGameStartFunc& onGameStartFunc);
		~RoomManager();

		RoomRef CreateRoom(UserRef user, WCHAR* roomName);
		void DestroyRoom(USHORT roomNum);

		RoomRef GetRoom(USHORT roomNum);
		
		std::vector<jh::RoomInfo> GetRoomInfoList();
	
	private:

		Room::UnicastFunc					m_unicastFunc;
		Room::OnGameStartFunc				m_onGameStartFunc;

		std::unordered_map<USHORT, RoomRef> m_roomMap;			// [roomNum, room]
		SRWLOCK								m_lock;
		
		USHORT								m_usMaxRoomCnt;
		USHORT								m_usMaxRoomUserCnt; // room당 최대 user 수

		alignas(64) SHORT					m_usUsingRoomCnt; // 
	};
}