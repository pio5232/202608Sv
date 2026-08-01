#include "pch.h"
#include "User.h"
#include "Room.h"

alignas(32) LONG jh::User::aliveLobbyUserCount = 0;

void jh::User::SetRoom(RoomRef room)
{
	if (nullptr != room)
	{
		m_usRoomNum = room->GetRoomNum();
		
		SetReadyState(false);

		m_bJoinFlag = true;
	}
	else
	{
		m_usRoomNum = 0;

		m_bJoinFlag = false;
	}
	m_myRoom = room;
}
