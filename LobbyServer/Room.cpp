#include "pch.h"
#include "Room.h"
#include "NetworkBase.h"
#include "RoomManager.h"
#include "PacketBuilder.h"
#include "User.h"
#include "Memory.h"

alignas(32) LONG jh::Room::aliveRoomCount = 0;

jh::Room::Room(const RoomInfo& roomInfo, const UnicastFunc& unicast, const OnEmptyFunc& onRoomEmpty, const OnGameStartFunc& onGameStart) :
	m_roomInfo{ roomInfo }, m_usReadyCnt{}, m_unicastFunc{ unicast }, m_onRoomEmpty{ onRoomEmpty }, m_onGameStart{ onGameStart }
{
	InterlockedIncrement(&aliveRoomCount);

}


jh::Room::~Room()
{
	InterlockedDecrement(&aliveRoomCount);
}

// 생성자에서 미처 하지 못했던 작업을 실행
void jh::Room::OnCreated(UserRef owner)
{
	PRO_START_AUTO_FUNC;

	m_userMap.insert(std::make_pair(owner->GetUserId(), owner));
	owner->SetRoom(static_pointer_cast<Room>(shared_from_this()));

	PacketBufferRef responsePkt = jh::PacketBuilder::BuildMakeRoomResponsePacket(GetRoomInfo());
	Unicast(responsePkt, owner->GetSessionId());

	std::vector<ULONGLONG> emptyVector;
	PacketBufferRef enterRoomResponsePkt = jh::PacketBuilder::BuildEnterRoomResponsePacket(true, emptyVector);
	Unicast(enterRoomResponsePkt, owner->GetSessionId());
}

void jh::Room::TryEnterRoom(UserRef user)
{
	PRO_START_AUTO_FUNC;

	if (true == IsRoomEmpty())
	{
		return;
	}

	if (m_roomInfo.m_usMaxUserCnt <= m_userMap.size())
	{
		PacketBufferRef errorPkt = jh::PacketBuilder::BuildErrorPacket(jh::PacketErrorCode::FULL_ROOM);

		Unicast(errorPkt, user->GetSessionId());

		return;	
	}

	if (RoomState::RUNNING == m_roomState)
	{
		PacketBufferRef errorPkt = jh::PacketBuilder::BuildErrorPacket(jh::PacketErrorCode::ALREADY_RUNNING_ROOM);
		
		Unicast(errorPkt, user->GetSessionId());

		return;
	}

	// 입장한 유저를 제외한 유저들의 정보를 모은다.
	std::vector<ULONGLONG> userIdAndReadyList;

	for (const auto& [_id, _userWptr] : m_userMap)
	{
		UserRef _user = _userWptr.lock();

		if (nullptr == _user)
			continue;

		ULONGLONG idAndReadyState = _id | ((ULONGLONG)_user->GetReadyState() << 63);

		userIdAndReadyList.push_back(idAndReadyState);
	}
	
	ULONGLONG userId = user->GetUserId();
	m_userMap.insert(std::make_pair(userId, user));
	user->SetRoom(static_pointer_cast<Room>(shared_from_this()));
	m_roomInfo.m_usCurUserCnt++;

	// 입장한 유저에게 기존의 유저 정보를 전송한다.
	PacketBufferRef enterRoomResponsePkt = jh::PacketBuilder::BuildEnterRoomResponsePacket(true, userIdAndReadyList);
	Unicast(enterRoomResponsePkt, user->GetSessionId());

	// 입장한 유저를 제외하고 입장 패킷을 보낸다.
	PacketBufferRef enterRoomNotifyPkt = jh::PacketBuilder::BuildEnterRoomNotifyPacket(userId);
	BroadCast(enterRoomNotifyPkt, userId);

	return;
}


void jh::Room::LeaveRoom(UserRef user)
{
	PRO_START_AUTO_FUNC;

	bool isReady = false;

	ULONGLONG userId = user->GetUserId();

	if (0 == ReleaseUser(user))
	{
		OnRoomEmpty();

		return;
	}
		
	PacketBufferRef leaveRoomNotifyPkt = jh::PacketBuilder::BuildLeaveRoomNotifyPacket(userId);
	BroadCast(leaveRoomNotifyPkt);

	// 방장이 나간 경우.
	if (userId == m_roomInfo.m_ullOwnerId)
	{
		// 위임한다.
		m_roomInfo.m_ullOwnerId = m_userMap.begin()->first;

		std::weak_ptr<User> ownerWptr = m_userMap[m_roomInfo.m_ullOwnerId];
		
		UserRef newOwnerUser = ownerWptr.lock();

		if (nullptr != newOwnerUser)
		{
			UpdateUserReadyStatus(newOwnerUser, false, false);

			PacketBufferRef sendBuffer = jh::PacketBuilder::BuildOwnerChangeNotifyPacket(newOwnerUser->GetUserId());

			BroadCast(sendBuffer);
		}
	}
	else
	{
		if (isReady)
			m_usReadyCnt--;
	}
	
}


/// <summary>
/// 유저 관리 해제
/// 남은 유저 수를 반환한다.
/// </summary>
USHORT jh::Room::ReleaseUser(UserRef user)
{
	
	user->SetRoom(nullptr);
	m_userMap.erase(user->GetUserId());
	m_roomInfo.m_usCurUserCnt--;

	PacketBufferRef leaveRoomResPkt = jh::PacketBuilder::BuildLeaveRoomResponsePacket();
	Unicast(leaveRoomResPkt, user->GetSessionId());
	
	return m_userMap.size();
}


void jh::Room::Chat(UserRef sender, PacketBufferRef packet)
{
	PRO_START_AUTO_FUNC;

	// 더미 테스트를 위해 ResponsePacket을 sender에게 전송.
	jh::PacketHeader responseHeader;
	responseHeader.type = jh::CHAT_TO_ROOM_RESPONSE_PACKET;

	PacketBufferRef chatResponsePkt = jh::MakeShared<PacketBuffer>(sizeof(responseHeader));

	*chatResponsePkt << responseHeader;
	Unicast(chatResponsePkt, sender->GetSessionId());

	BroadCast(packet);
}

void jh::Room::UpdateUserReadyStatus(UserRef user, bool isReady, bool sendOpt) // Ready 정보를 모두에게 알릴 것인가. (방장 교체의 경우 false => 알리지 않는다.)
{
	PRO_START_AUTO_FUNC;
	
	_LOG(LOBBY_SYSTEM_SAVE_FILE_NAME, LOG_LEVEL_NO_LOG, L"[UpdateUserReadyStatus] UserId : [%llu], User->IsReady : [%d], isready : [%d]", user->GetUserId(), user->GetReadyState(), isReady);

	if (user->GetReadyState() == isReady)
		return;

	user->SetReadyState(isReady);
	 
	if (true == isReady)
		m_usReadyCnt++;
	else
		m_usReadyCnt--;

	ULONGLONG userId = user->GetUserId();

	if (m_roomInfo.m_ullOwnerId == userId && GetCurUserCnt() == m_usReadyCnt && RoomState::IDLE == m_roomState)
	{
		m_roomState = RoomState::RUNNING;

		PacketBufferRef gameStartNotifyPkt = jh::PacketBuilder::BuildGameStartNotifyPacket();

		BroadCast(gameStartNotifyPkt);

		OnGameStart();
		
		return;
	}

	if (sendOpt)
	{
		PacketBufferRef gameReadyNotifyPkt = jh::PacketBuilder::BuildGameReadyNotifyPacket(userId, isReady);

		BroadCast(gameReadyNotifyPkt);
	}
}

void jh::Room::BroadCast(PacketBufferRef packet,const ULONGLONG excludedUserId)
{
	PRO_START_AUTO_FUNC;
	
	for (const auto& [userId, userWptr] : m_userMap)
	{
		UserRef user = userWptr.lock();

		if (nullptr == user)
			continue;

		if (excludedUserId == userId)
			continue;

		ULONGLONG _sessionId = user->GetSessionId();

		Unicast(packet, _sessionId);
	}
		
}

