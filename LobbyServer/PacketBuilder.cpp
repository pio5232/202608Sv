#include "pch.h"
#include "PacketBuilder.h"
#include "Memory.h"
#include "SerializationBuffer.h"

PacketBufferRef jh::PacketBuilder::BuildErrorPacket(jh::PacketErrorCode errorCode)
{
	PRO_START_AUTO_FUNC;
	jh::ErrorPacket errorPacket;

	errorPacket.packetErrorCode = errorCode;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(errorPacket));

	*buffer << errorPacket.size << errorPacket.type << errorPacket.packetErrorCode;

	return buffer;

}

PacketBufferRef jh::PacketBuilder::BuildLogInResponsePacket(ULONGLONG userId)
{
	PRO_START_AUTO_FUNC;
	jh::LogInResponsePacket logInResponsePacket; 

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(logInResponsePacket));

	*buffer << logInResponsePacket.size << logInResponsePacket.type << userId;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildMakeRoomResponsePacket(const RoomInfo& roomInfo)
{
	PRO_START_AUTO_FUNC;
	jh::MakeRoomResponsePacket makeRoomResponsePacket;

	makeRoomResponsePacket.isMade = true;
	makeRoomResponsePacket.roomInfo = roomInfo;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(makeRoomResponsePacket));

	*buffer << makeRoomResponsePacket.size << makeRoomResponsePacket.type << makeRoomResponsePacket.isMade << makeRoomResponsePacket.roomInfo;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildEnterRoomNotifyPacket(ULONGLONG userId)
{
	PRO_START_AUTO_FUNC;
	jh::EnterRoomNotifyPacket enterRoomNotifyPacket;

	enterRoomNotifyPacket.enterUserId = userId;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(enterRoomNotifyPacket));

	*buffer << enterRoomNotifyPacket.size << enterRoomNotifyPacket.type << enterRoomNotifyPacket.enterUserId;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildEnterRoomResponsePacket(bool isAllowed, const std::vector<ULONGLONG>& userIdAndReadyList)
{
	PRO_START_AUTO_FUNC;
	jh::EnterRoomResponsePacket enterRoomResponsePacket;

	enterRoomResponsePacket.bAllow = isAllowed;
	enterRoomResponsePacket.idCnt = userIdAndReadyList.size();

	enterRoomResponsePacket.size = sizeof(bool) + sizeof(USHORT) + sizeof(ULONGLONG) * userIdAndReadyList.size();
	
	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(enterRoomResponsePacket) + enterRoomResponsePacket.size);

	*buffer << enterRoomResponsePacket.size << enterRoomResponsePacket.type << enterRoomResponsePacket.bAllow << enterRoomResponsePacket.idCnt;

	for (ULONGLONG userId : userIdAndReadyList)
	{
		*buffer << userId;
	}

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildLeaveRoomNotifyPacket(ULONGLONG userId)
{
	PRO_START_AUTO_FUNC;
	jh::LeaveRoomNotifyPacket leaveRoomNotifyPacket;

	leaveRoomNotifyPacket.leaveUserId = userId;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(leaveRoomNotifyPacket));

	*buffer << leaveRoomNotifyPacket.size << leaveRoomNotifyPacket.type << leaveRoomNotifyPacket.leaveUserId;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildLeaveRoomResponsePacket()
{
	PRO_START_AUTO_FUNC;
	jh::LeaveRoomResponsePacket leaveRoomResPacket;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(leaveRoomResPacket));

	*buffer << leaveRoomResPacket.size << leaveRoomResPacket.type;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildRoomListResponsePacket(std::vector<jh::RoomInfo>& roomInfoList)
{
	PRO_START_AUTO_FUNC;
	jh::PacketHeader header;

	USHORT roomCnt = static_cast<USHORT>(roomInfoList.size());
	header.size = sizeof(roomCnt) + roomCnt * jh::RoomInfo::GetSize();
	header.type = jh::ROOM_LIST_RESPONSE_PACKET;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(jh::RoomListResponsePacket) + roomCnt * jh::RoomInfo::GetSize());

	*buffer << header.size << header.type << roomCnt;

	for (const jh::RoomInfo& roomInfo : roomInfoList)
	{
		*buffer << roomInfo.m_ullOwnerId << roomInfo.m_usRoomNum << roomInfo.m_usCurUserCnt << roomInfo.m_usMaxUserCnt;

		buffer->PutData(reinterpret_cast<const char*>(roomInfo.m_wszRoomName), ROOM_NAME_MAX_LEN * MESSAGE_SIZE);
	}

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildOwnerChangeNotifyPacket(ULONGLONG newOwnerUserId)
{
	PRO_START_AUTO_FUNC;
	jh::OwnerChangeNotifyPacket ownerChangeNotifyPacket;
	ownerChangeNotifyPacket.userId = newOwnerUserId;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(ownerChangeNotifyPacket));

	*buffer << ownerChangeNotifyPacket.size << ownerChangeNotifyPacket.type << ownerChangeNotifyPacket.userId;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildGameStartNotifyPacket()
{
	PRO_START_AUTO_FUNC;
	jh::GameStartNotifyPacket gameStartNotifyPacket;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(gameStartNotifyPacket));

	*buffer << gameStartNotifyPacket.size << gameStartNotifyPacket.type;
	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildGameReadyNotifyPacket(ULONGLONG userId, bool isReady)
{
	PRO_START_AUTO_FUNC;
	jh::GameReadyNotifyPacket gameReadyNotifyPacket;

	gameReadyNotifyPacket.userId = userId;
	gameReadyNotifyPacket.isReady = isReady;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(gameReadyNotifyPacket));

	*buffer << gameReadyNotifyPacket.size << gameReadyNotifyPacket.type << gameReadyNotifyPacket.isReady << gameReadyNotifyPacket.userId;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildEchoPacket()
{
	PRO_START_AUTO_FUNC;
	jh::EchoPacket echoPacket;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(echoPacket));

	*buffer << echoPacket.size << echoPacket.type << jh_utility::GetTimeStamp();

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildLanInfoPacket(WCHAR* ipStr, USHORT port, USHORT roomNum, ULONGLONG tok)
{
	PRO_START_AUTO_FUNC;
	jh::GameServerLanInfoPacket gameServerLanInfoPacket;
	
	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(gameServerLanInfoPacket));
	
	*buffer << gameServerLanInfoPacket.size << gameServerLanInfoPacket.type;

	buffer->PutData(reinterpret_cast<const char*>(ipStr), IP_STRING_LEN * MESSAGE_SIZE);
	
	*buffer << port << roomNum << tok;


	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildGameServerSettingResponsePacket(USHORT roomNum, USHORT requiredUserCnt, USHORT maxUserCnt)
{
	PRO_START_AUTO_FUNC;
	jh::GameServerSettingResponsePacket gameServerSettingResponsePacket;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(gameServerSettingResponsePacket));

	*buffer << gameServerSettingResponsePacket.size << gameServerSettingResponsePacket.type << roomNum << requiredUserCnt << maxUserCnt;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildChatNotifyPacket(ULONGLONG senderId, USHORT messageLen, const char* msg)
{
	PRO_START_AUTO_FUNC;
	jh::PacketHeader packetHeader;

	// --- NotifyPacket
	packetHeader.size = sizeof(senderId) + sizeof(messageLen) + messageLen;
	packetHeader.type = jh::CHAT_NOTIFY_PACKET;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(packetHeader) + packetHeader.size);

	*buffer << packetHeader << senderId << messageLen;

	buffer->PutData(reinterpret_cast<const char*>(msg), messageLen);
	
	return buffer;
}

