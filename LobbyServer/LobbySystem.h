#pragma once
#include "Timer.h"

namespace jh
{	
	class LobbySystem
	{
		using PacketFunc = void(LobbySystem::*)(ULONGLONG, PacketBufferRef&);
		using UpdateHbFuncType = std::function<void(ULONGLONG, ULONGLONG)>;
	public:
		LobbySystem(jh::IocpServer* owner, USHORT maxRoomCnt, USHORT maxRoomUserCnt, UpdateHbFuncType hbFunc);
		~LobbySystem();

		void Init();
		// 로직 스레드의 실행을 종료
		void Stop();

		void GetInvalidMsgCnt() const;
		void ProcessPacket(ULONGLONG sessionId, USHORT packetType, PacketBufferRef& packet);
	private:
		LONGLONG m_invalidLeave = 0;
		LONGLONG m_invalidEnter = 0;
		LONGLONG m_invalidChat = 0;
		LONGLONG m_invalidMake = 0;
		LONGLONG m_invalidReady = 0;

		// 패킷 처리 함수
		void HandleRoomListRequestPacket(ULONGLONG sessionId,PacketBufferRef& packet);
		void HandleLogInRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleChatToRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleMakeRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleEnterRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleLeaveRoomRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleGameReadyRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleEchoPacket(ULONGLONG sessionId, PacketBufferRef& packet);

		void HandleHeartbeatPacket(ULONGLONG sessionId, PacketBufferRef& packet);
	public:
		// Lan Request
		void HandleLanInfoNotify(ULONGLONG lanSessionId, PacketBufferRef& lanPacket, jh::IocpServer* lanServer);
		void HandleGameSettingRequest(ULONGLONG lanSessionId, PacketBufferRef& lanPacket, jh::IocpServer* lanServer);
		
	public:
		void DisconnectUser(ULONGLONG sessionId) const;

	private:
		std::unordered_map<USHORT, PacketFunc>		m_packetFuncDic;

		jh::UniquePtr<jh::UserManager>				m_pUserManager;
		jh::UniquePtr<jh::RoomManager>				m_pRoomManager;

		jh::IocpServer								* m_pOwner;
		UpdateHbFuncType							m_updateHeartbeatFunc;
		jh_utility::LockQueue<USHORT>				m_pendingGameRoomList;
	};
}