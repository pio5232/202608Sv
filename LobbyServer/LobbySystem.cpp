#include "pch.h"
#include "LobbySystem.h"
#include "RoomManager.h"
#include "UserManager.h"
#include "NetworkBase.h"
#include "User.h"
#include "Room.h"
void jh::LobbySystem::Init()
{
	m_pUserManager->Init();

	m_packetFuncDic.clear();

	m_packetFuncDic[jh::ROOM_LIST_REQUEST_PACKET] = &LobbySystem::HandleRoomListRequestPacket;
	m_packetFuncDic[jh::CHAT_TO_ROOM_REQUEST_PACKET] = &LobbySystem::HandleChatToRoomRequestPacket;
	m_packetFuncDic[jh::LOG_IN_REQUEST_PACKET] = &LobbySystem::HandleLogInRequestPacket;
	m_packetFuncDic[jh::MAKE_ROOM_REQUEST_PACKET] = &LobbySystem::HandleMakeRoomRequestPacket;
	m_packetFuncDic[jh::ENTER_ROOM_REQUEST_PACKET] = &LobbySystem::HandleEnterRoomRequestPacket;
	m_packetFuncDic[jh::LEAVE_ROOM_REQUEST_PACKET] = &LobbySystem::HandleLeaveRoomRequestPacket;
	m_packetFuncDic[jh::GAME_READY_REQUEST_PACKET] = &LobbySystem::HandleGameReadyRequestPacket;
	m_packetFuncDic[jh::HEART_BEAT_PACKET] = &LobbySystem::HandleHeartbeatPacket;
	m_packetFuncDic[jh::ECHO_PACKET] = &LobbySystem::HandleEchoPacket;
}

void jh::LobbySystem::Stop()
{
}

void jh::LobbySystem::GetInvalidMsgCnt() const
{
	wprintf(L" [Content] Invalid Leave : %lld\n", m_invalidLeave);
	wprintf(L" [Content] Invalid Enter : %lld\n", m_invalidEnter);
	wprintf(L" [Content] Invalid Chat : %lld\n", m_invalidChat);
	wprintf(L" [Content] Invalid Make : %lld\n", m_invalidMake);
	wprintf(L" [Content] Invalid Ready : %lld\n", m_invalidReady);

}

jh::LobbySystem::LobbySystem(jh::IocpServer* owner, USHORT maxRoomCnt, USHORT maxRoomUserCnt, UpdateHbFuncType hbFunc) : m_pOwner{ owner }, m_pendingGameRoomList{}, m_updateHeartbeatFunc{hbFunc}
{
	if (nullptr == m_pOwner)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[LobbySystem] m_pOwner is nullptr");
		jh_utility::CrashDump::Crash();
	}

	m_pUserManager = jh::MakeUnique<jh::UserManager>();


	auto unicastFunc = [this](ULONGLONG _sessionId, PacketBufferRef _packet)
		{
			m_pOwner->SendPacket(_sessionId, _packet);
		};

	auto onGameStartFunc = [this](USHORT _roomNum)
		{
			m_pendingGameRoomList.Push(_roomNum);
			
			std::wstring path(GAME_FILE_PATH);

			ExecuteProcess(GAME_FILE_PATH, GAME_CUR_DIRECTORY);
		};

	m_pRoomManager = jh::MakeUnique<jh::RoomManager>(maxRoomCnt, maxRoomUserCnt, unicastFunc, onGameStartFunc);
}

LobbySystem::~LobbySystem()
{
}


void jh::LobbySystem::ProcessPacket(ULONGLONG sessionId, USHORT packetType, PacketBufferRef& packet)
{
	if (m_packetFuncDic.find(packetType) == m_packetFuncDic.end())
		return;

	(this->*m_packetFuncDic[packetType])(sessionId, packet);
}


void jh::LobbySystem::HandleRoomListRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PRO_START_AUTO_FUNC;

	UserRef user = m_pUserManager->GetUserBySessionId(sessionId);

	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleRoomListRequestPacket] User not found. SessionId : [0x%016llx]", sessionId);

		return;
	}

	std::vector<jh::RoomInfo> roomInfoList = m_pRoomManager->GetRoomInfoList(); // Get list from RoomManager

	PacketBufferRef responsePkt = PacketBuilder::BuildRoomListResponsePacket(roomInfoList);
	
	m_pOwner->SendPacket(sessionId, responsePkt);
}

void jh::LobbySystem::HandleLogInRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PRO_START_AUTO_FUNC;

	ULONGLONG id;
	ULONGLONG pw;

	*packet >> id >> pw;

	UserRef user = m_pUserManager->CreateUser(sessionId);
	
	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleLogInRequestPacket] Duplicate SessionId: [0x%016llx]", sessionId);
		return;
	}

	PacketBufferRef logInResponsePkt = jh::PacketBuilder::BuildLogInResponsePacket(user->GetUserId());

	m_pOwner->SendPacket(sessionId, logInResponsePkt);
}
void jh::LobbySystem::HandleChatToRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PRO_START_AUTO_FUNC;

	USHORT messageLen; // WCHAR
	USHORT roomNum;

	*packet >> roomNum >> messageLen;
	
	std::shared_ptr<char> payLoad(static_cast<char*>(g_pMemSystem->Alloc(messageLen)),[](char* p) { g_pMemSystem->Free(p); });
	
	packet->GetData(payLoad.get(), messageLen);

	UserRef user = m_pUserManager->GetUserBySessionId(sessionId);

	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleChatToRoomRequestPacket] User not found. SessionId : [0x%016llx]", sessionId);

		return;
	}

	auto [isAlreadyInRoom, serverRoomNum] = user->TryGetRoomId();

	if (false == isAlreadyInRoom || serverRoomNum != roomNum)
	{
		m_invalidChat++;

		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleChatToRoomRequestPacket] Invalid room. UserId : [%llu], ClientRoom : [%hu], ServerRoom: [%hu]", user->GetUserId(), roomNum, serverRoomNum);

		return;
	}

	RoomRef room = m_pRoomManager->GetRoom(roomNum);

	if (nullptr == room)
		return;

	PacketBufferRef chatNotifyPkt = jh::PacketBuilder::BuildChatNotifyPacket(user->GetUserId(), messageLen, payLoad.get());

	room->DoAsync(&Room::Chat,user, chatNotifyPkt);
}

void jh::LobbySystem::HandleMakeRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PRO_START_AUTO_FUNC;

	WCHAR roomName[ROOM_NAME_MAX_LEN];

	packet->GetData(reinterpret_cast<char*>(roomName), sizeof(roomName));

	UserRef user = m_pUserManager->GetUserBySessionId(sessionId);

	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_INFO, L"[HandleMakeRoomRequestPacket] User not found. SessionID : [0x%016llx].", sessionId);

		return;
	}

	auto [isAlreadyInRoom, roomNum] = user->TryGetRoomId();
	
	if (true == isAlreadyInRoom)
	{
		m_invalidMake++;
		
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleMakeRoomRequestPacket] User already in room. SessionId : [0x%016llx], ServerRoom: [%hu]", sessionId, roomNum);

		return;
	}

	RoomRef newRoom = m_pRoomManager->CreateRoom(user, roomName);

	if (nullptr == newRoom)
	{
		jh::MakeRoomResponsePacket makeRoomResponsePkt;
		
		makeRoomResponsePkt.isMade = false;
		
		PacketBufferRef responsePkt = jh::MakeShared<PacketBuffer>(sizeof(makeRoomResponsePkt));

		*responsePkt << makeRoomResponsePkt;

		m_pOwner->SendPacket(sessionId, responsePkt);

		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_NO_LOG, L"[HandleMakeRoomRequestPacket] Create room failed. SessionId : [0x%016llx], ServerRoom: [%hu]", sessionId, roomNum);

		return;
	}
}

void jh::LobbySystem::HandleEnterRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PRO_START_AUTO_FUNC;

	USHORT roomNum;
	WCHAR roomName[ROOM_NAME_MAX_LEN];

	*packet >> roomNum;
	packet->GetData(reinterpret_cast<char*>(roomName), sizeof(roomName));

	UserRef user = m_pUserManager->GetUserBySessionId(sessionId);
		
	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_INFO, L"[HandleEnterRoomRequestPacket] User not found. SessionID: [0x%016llx]", sessionId);
		
		return;
	}

	RoomRef room = m_pRoomManager->GetRoom(roomNum);

	// 잘못된 방인지 체크.
	if (nullptr == room)
	{
		PacketBufferRef errorPkt = jh::PacketBuilder::BuildErrorPacket(jh::PacketErrorCode::REQUEST_DESTROYED_ROOM);

		m_pOwner->SendPacket(sessionId, errorPkt);

		return;
	}

	if (wcscmp(roomName, room->GetRoomNamePtr()) != 0)
	{
		PacketBufferRef errorPkt = jh::PacketBuilder::BuildErrorPacket(jh::PacketErrorCode::REQUEST_DIFF_ROOM_NAME);

		m_pOwner->SendPacket(sessionId, errorPkt);

		return;
	}

	auto [isAlreadyInRoom, curRoomNum] = user->TryGetRoomId();

	if (true == isAlreadyInRoom)
	{
		m_invalidEnter++;

		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleEnterRoomRequestPacket] User already in room. SessionId: [0x%016llx], ClientRoom: [%hu], ServerRoom: [%hu]", sessionId, roomNum, curRoomNum);

		return;
	}

	if (wcscmp(roomName, room->GetRoomNamePtr()) != 0)
	{
		m_invalidEnter++;

		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleEnterRoomRequestPacket] different roomname. SessionId: [0x%016llx], ClientRoom: [%hu], ServerRoom: [%hu]", sessionId, roomNum, curRoomNum);

		return;
	}
	room->DoAsync(&Room::TryEnterRoom, user);

}
void jh::LobbySystem::HandleLeaveRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PRO_START_AUTO_FUNC;

	jh::LeaveRoomRequestPacket leaveRoomRequestPacket;

	*packet >> leaveRoomRequestPacket;

	UserRef user = m_pUserManager->GetUserBySessionId(sessionId);

	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_INFO, L"[HandleLeaveRoomRequestPacket] User not found. SessionId : [%llu]", sessionId);

		return;
	}

	RoomRef room = m_pRoomManager->GetRoom(leaveRoomRequestPacket.roomNum);

	if (nullptr == room)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_INFO, L"[HandleLeaveRoomRequestPacket] Room not found. Room: [%hu], SessionId : [0x%016llx], UserId : [%llu] ", leaveRoomRequestPacket.roomNum, sessionId,user->GetUserId());

		return;
	}

	const auto [isAlreadyInRoom, roomNum] = user->TryGetRoomId();

	if (false == isAlreadyInRoom || roomNum != leaveRoomRequestPacket.roomNum)
	{
		m_invalidLeave++;

		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L" [HandleLeaveRoomRequestPacket] - UserId : [%llu], ClientRoomNum : [%hu], ServerRoomNum : [%hu], IsAlreadyInRoom : [%s]", user->GetUserId(), leaveRoomRequestPacket.roomNum, roomNum, isAlreadyInRoom ? "true" : "false");

		return;
	}

	if (wcscmp(leaveRoomRequestPacket.roomName, room->GetRoomNamePtr()) != 0)
	{
		m_invalidLeave++;

		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleLeaveRoomRequestPacket] different roomname. SessionId: [0x%016llx], ClientRoom: [%hu], ServerRoom: [%hu]", sessionId, leaveRoomRequestPacket.roomNum, roomNum);

		return;
	}

	room->DoAsync(&Room::LeaveRoom, user);
}

void jh::LobbySystem::HandleGameReadyRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PRO_START_AUTO_FUNC;

	jh::GameReadyRequestPacket requestPacket;

	*packet >> requestPacket.isReady;

	UserRef user = m_pUserManager->GetUserBySessionId(sessionId);

	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[GameReadyRequest] User not found. SessionID: [0x%016llx]", sessionId);
		
		return;
	}

	const auto [res, roomNum] = user->TryGetRoomId();

	if (false == res)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[GameReadyRequest] User not in room. UserId: [%llu]", user->GetUserId());

		m_invalidReady++;
		
		return;
	}
	
	RoomRef room = m_pRoomManager->GetRoom(roomNum);

	if (nullptr != room)
		room->DoAsync(&Room::UpdateUserReadyStatus, user, requestPacket.isReady, true);
}

void jh::LobbySystem::HandleEchoPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	PacketBufferRef echoPkt = jh::PacketBuilder::BuildEchoPacket();
	
	m_pOwner->SendPacket(sessionId, echoPkt);

	return;
}

void jh::LobbySystem::HandleHeartbeatPacket(ULONGLONG sessionId, PacketBufferRef& packet) 
{
	PRO_START_AUTO_FUNC;

	ULONGLONG packetTimeStamp;

	*packet >> packetTimeStamp;

	m_updateHeartbeatFunc(sessionId, packetTimeStamp);

	return;
}

void jh::LobbySystem::HandleLanInfoNotify(ULONGLONG lanSessionId, PacketBufferRef& lanPacket, jh::IocpServer* lanServer)
{
	PRO_START_AUTO_FUNC;

	printf("Lan Info Notify Packet Recv\n");

	WCHAR ipStr[IP_STRING_LEN] = {};
	USHORT port = 0;
	USHORT roomNum = 0;
	ULONGLONG xorToken = 0;

	lanPacket->GetData(reinterpret_cast<char*>(ipStr), sizeof(ipStr));

	*lanPacket >> port >> roomNum >> xorToken;

	//wprintf(L"ip: [ %s ]\n", ipStr);
	//wprintf(L"port : [ %d ]\n", port);
	//wprintf(L"Room : [ %d ]\n", roomNum);
	//wprintf(L"xorToken : [ %llu ], After : [%llu]\n", xorToken, xorToken ^ xorTokenKey);

	PacketBufferRef buildLanInfoPkt = jh::PacketBuilder::BuildLanInfoPacket(ipStr, port, roomNum, xorToken ^ xorTokenKey);

	RoomRef room = m_pRoomManager->GetRoom(roomNum);

	if (nullptr == room)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleLanInfoNotify] Room not found: [%hu]", roomNum);

		return;
	}

	room->DoAsync(&Room::BroadCast,buildLanInfoPkt, 0);
}

void jh::LobbySystem::HandleGameSettingRequest(ULONGLONG lanSessionId, PacketBufferRef& lanPacket, jh::IocpServer* lanServer)
{
	PRO_START_AUTO_FUNC;

	_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_INFO, L"[HandleGameSettingRequest] GameSetting RequestPacket Recv");

	printf("Game Setting Request Packet Recv\n");

	USHORT pendingRoomNum;
	bool ret = m_pendingGameRoomList.TryPop(pendingRoomNum);
	if (false == ret)
		return;

	RoomRef room = m_pRoomManager->GetRoom(pendingRoomNum);

	if (nullptr == room)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[HandleGameSettingRequest] Room not found: [%hu]", pendingRoomNum);
		return;
	}

	room->DoAsync(
		[self = room, lanServer, lanSessionId]()
		{
			USHORT requiredUserCnt = self->GetCurUserCnt();
			USHORT maxUserCnt = self->GetMaxUserCnt();
			USHORT roomNum = self->GetRoomNum();

			PacketBufferRef gameServerSettingResponsePkt = jh::PacketBuilder::BuildGameServerSettingResponsePacket(roomNum, requiredUserCnt, maxUserCnt);
			lanServer->SendPacket(lanSessionId, gameServerSettingResponsePkt);
		}
	);
}

void jh::LobbySystem::DisconnectUser(ULONGLONG sessionId) const
{
	PRO_START_AUTO_FUNC;

	UserRef user = m_pUserManager->GetUserBySessionId(sessionId);

	if (nullptr == user)
	{
		_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_INFO, L"[DisconnectUser] User not found on disconnect. SessionId: [0x%016llx]", sessionId);
		return;
	}

	const auto [isJoined, roomNum] = user->TryGetRoomId();

	if (true == isJoined)
	{
		RoomRef room = m_pRoomManager->GetRoom(roomNum);

		if (nullptr == room)
		{
			_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_WARNING, L"[DisconnectUser] User room mismatch. RoomNum : [%hu], UserID : [%llu]", roomNum, user->GetUserId());
		}
		else
		{
			room->DoAsync(
				[self = room, user, sessionId, userMgr = m_pUserManager.get()]() {

					self->LeaveRoom(user);

					userMgr->RemoveUser(sessionId);

				});
		}
	}
	else
		m_pUserManager->RemoveUser(sessionId);

}
