#include "pch.h"
#include "User.h"

alignas(64) ULONGLONG jh::User::_aliveLobbyUserCount = 0;
const std::pair<bool, USHORT> jh::User::GetRoomId() const
{
	return { _isJoinedRoom, _roomNum };
}
