#include "pch.h"
#include "PacketBuilder.h"
#include "Memory.h"

PacketBufferRef jh::PacketBuilder::BuildErrorPacket(jh::PacketErrorCode errorCode)
{
	jh::ErrorPacket errorPacket;

	errorPacket.packetErrorCode = errorCode;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(errorPacket));

	*buffer << errorPacket.size << errorPacket.type << errorPacket.packetErrorCode;

	return buffer;
}

PacketBufferRef jh::PacketBuilder::BuildAttackNotifyPacket(ULONGLONG entityId)
{
	jh::AttackNotifyPacket attackNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(attackNotifyPacket));

	*sendBuffer << attackNotifyPacket.size << attackNotifyPacket.type << entityId;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildAttackedNotifyPacket(ULONGLONG entityId, USHORT currentHp)
{
	jh::AttackedNotifyPacket attackedNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(attackedNotifyPacket));

	*sendBuffer << attackedNotifyPacket.size << attackedNotifyPacket.type << entityId << currentHp;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildMakeMyCharacterPacket(ULONGLONG entityId, const Vector3& position)
{
	jh::MakeMyCharacterPacket makeMyCharacterPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(makeMyCharacterPacket));

	*sendBuffer << makeMyCharacterPacket.size << makeMyCharacterPacket.type << entityId << position;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildMakeOtherCharacterPacket(ULONGLONG entityId, const Vector3& position)
{
	jh::MakeOtherCharacterPacket makeOtherCharacterPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(makeOtherCharacterPacket));

	*sendBuffer << makeOtherCharacterPacket.size << makeOtherCharacterPacket.type << entityId << position;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildGameInitDonePacket()
{
	jh::GameInitDonePacket donePacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(donePacket));

	*sendBuffer << donePacket.size << donePacket.type;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildDeleteOtherCharacterPacket(ULONGLONG entityId)
{
	jh::DeleteOtherCharacterPacket deleteOtherCharacterPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(deleteOtherCharacterPacket));

	*sendBuffer << deleteOtherCharacterPacket.size << deleteOtherCharacterPacket.type << entityId;
	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildMoveStartNotifyPacket(ULONGLONG entityId, const Vector3& pos, float rotY)
{
	jh::MoveStartNotifyPacket moveStartNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(moveStartNotifyPacket));

	*sendBuffer << moveStartNotifyPacket.size << moveStartNotifyPacket.type << entityId << pos << rotY;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildMoveStopNotifyPacket(ULONGLONG entityId, const Vector3& pos, float rotY)
{
	jh::MoveStopNotifyPacket moveStopNotifyPacket;
	moveStopNotifyPacket.entityId = entityId;
	moveStopNotifyPacket.pos = pos;
	moveStopNotifyPacket.rotY = rotY;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(moveStopNotifyPacket));

	*sendBuffer << moveStopNotifyPacket.size << moveStopNotifyPacket.type << moveStopNotifyPacket.entityId
		<< moveStopNotifyPacket.pos << moveStopNotifyPacket.rotY;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildUpdateTransformPacket(ULONGLONG timeStamp, ULONGLONG entityId, const Vector3& pos, const Vector3& rot)
{
	jh::UpdateTransformPacket updatePacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(updatePacket));

	*sendBuffer << updatePacket.size << updatePacket.type << timeStamp << entityId << pos << rot;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildDieNotifyPacket(ULONGLONG entityId)
{
	jh::DieNotifyPacket dieNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(dieNotifyPacket));

	*sendBuffer << dieNotifyPacket.size << dieNotifyPacket.type << entityId;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildSpectatorInitPacket()
{
	jh::SpectatorInitPacket spectatorInitPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(spectatorInitPacket));

	*sendBuffer << spectatorInitPacket.size << spectatorInitPacket.type;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildEnterGameResponsePacket()
{
	jh::EnterGameResponsePacket enterGameResponsePacket;

	PacketBufferRef buffer = jh::MakeShared<PacketBuffer>(sizeof(enterGameResponsePacket));

	*buffer << enterGameResponsePacket.size << enterGameResponsePacket.type;

	return buffer;
}


PacketBufferRef jh::PacketBuilder::BuildGameEndNotifyPacket()
{
	jh::GameEndNotifyPacket gameEndNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(gameEndNotifyPacket));

	*sendBuffer << gameEndNotifyPacket.size << gameEndNotifyPacket.type;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildUpdateWinnerNotifyPacket(ULONGLONG userId, ULONGLONG expectedTimeStamp)
{
	jh::UpdateWinnerNotifyPacket updateWinnerNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(updateWinnerNotifyPacket));

	*sendBuffer << updateWinnerNotifyPacket.size << updateWinnerNotifyPacket.type << userId << expectedTimeStamp;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildInvalidateWinnerNotifyPacket(ULONGLONG canceledUserID)
{
	jh::InvalidateWinnerNotifyPacket invalidateWinnerNotifyPacket;
	invalidateWinnerNotifyPacket.canceledUserId = canceledUserID;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(invalidateWinnerNotifyPacket));

	*sendBuffer << invalidateWinnerNotifyPacket.size << invalidateWinnerNotifyPacket.type << invalidateWinnerNotifyPacket.canceledUserId;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildWinnerInfoNotifyPacket()
{
	jh::WinnerInfoNotifyPacket winnerInfoNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(winnerInfoNotifyPacket));

	*sendBuffer << winnerInfoNotifyPacket.size << winnerInfoNotifyPacket.type;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildCharacterSyncPacket(ULONGLONG entityId, const Vector3& syncPos, const Vector3& syncRot)
{
	jh::CharacterPositionSyncPacket syncPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(syncPacket));

	*sendBuffer << syncPacket.size << syncPacket.type << entityId << syncPos << syncRot;
	
	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildGameStartNotifyPacket()
{
	jh::GameStartNotifyPacket gameStartNotifyPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(gameStartNotifyPacket));

	*sendBuffer << gameStartNotifyPacket.size << gameStartNotifyPacket.type;
	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildGameServerSettingRequestPacket()
{
	jh::GameServerSettingRequestPacket settingRequestPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(settingRequestPacket));

	*sendBuffer << settingRequestPacket.size << settingRequestPacket.type;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildGameServerLanInfoPacket(const WCHAR* ipStr, USHORT port, USHORT roomNum, ULONGLONG token)
{
	jh::GameServerLanInfoPacket gameServerLanInfoPacket;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(gameServerLanInfoPacket));

	wcscpy_s(gameServerLanInfoPacket.ipStr, IP_STRING_LEN, ipStr);

	*sendBuffer << gameServerLanInfoPacket.size << gameServerLanInfoPacket.type;

	sendBuffer->PutData(reinterpret_cast<const char*>(gameServerLanInfoPacket.ipStr), IP_STRING_LEN * MESSAGE_SIZE);

	*sendBuffer << port << roomNum << token;

	return sendBuffer;
}

PacketBufferRef jh::PacketBuilder::BuildChatNotifyPacket(ULONGLONG userId, USHORT messageLen, const char* message)
{
	jh::ChatOtherUserNotifyPacket chatNotifyPacket;

	chatNotifyPacket.size = sizeof(userId) + sizeof(messageLen) + messageLen;
	chatNotifyPacket.type = jh::CHAT_NOTIFY_PACKET;

	PacketBufferRef sendBuffer = jh::MakeShared<PacketBuffer>(sizeof(jh::PacketHeader) + chatNotifyPacket.size);
	*sendBuffer << chatNotifyPacket.size << chatNotifyPacket.type << userId << messageLen;

	sendBuffer->PutData(message, messageLen);

	return sendBuffer;
}
