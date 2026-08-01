#pragma once

namespace jh
{
	class GameWorld;
	class UserManager;

	struct GameServerInfo
	{
		ULONGLONG m_ullEnterToken;
		USHORT m_usRoomNumber;

		USHORT m_usRequiredUserCnt;
		USHORT m_usMaxUserCnt;
	};

	class GameSystem
	{
		using PacketFunc = void(GameSystem::*)(ULONGLONG, PacketBufferRef&);

	public:
		GameSystem(jh::IocpServer* owner);
		~GameSystem();

		USHORT GetRoomNumber() const { return m_gameInfo.m_usRoomNumber; }
		ULONGLONG GetToken() const { return m_gameInfo.m_ullEnterToken; }
		USHORT GetRequiredUsers() const { return m_gameInfo.m_usRequiredUserCnt; }
		USHORT MaxUsers() const { return m_gameInfo.m_usMaxUserCnt; }

		// Lan에서 정보를 받으면 갱신한다.
		void SetGameInfo(USHORT roomNumber, USHORT requiredUsers, USHORT maxUsers);

		void EnqueueLogicEvent(LogicEventRef& logicEventPtr)
		{
			m_netLogicEventQueue.Push(logicEventPtr);
		}
		void EnqueueSessionConnEvent(SessionConnectionEventRef& sessionConnectionEventPtr)
		{
			m_sessionConnEventQueue.Push(sessionConnectionEventPtr);
		}
		void EnqueueLanRequest(GameLanEventPtr& lanRequestPtr)
		{
			m_gameLanRequestQueue.Push(lanRequestPtr);
		}
		void Init();
		void Stop();
	private:
		static unsigned LogicThreadFunc(LPVOID lparam);
		void GameLogic();

		void ProcessPacket(ULONGLONG sessionId, DWORD packetType, PacketBufferRef& packet);

		void ProcessLogicEvent();
		void ProcessSessionConnectionEvent();
		void ProcessLanRequest();

		// GAME
		void HandleEnterGameRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleLoadCompletedPacket(ULONGLONG sessionId, PacketBufferRef& packet);

		void HandleMoveStartRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleMoveStopRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleChatRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);

		void HandleAttackRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);

		// LAN
		void HandleGameServerSettingResponsePacket(ULONGLONG lanSessionId, PacketBufferRef& packet, jh::IocpClient* lanClient);
	private:
		GameServerInfo										m_gameInfo;

		char												m_bIsRunning;
		USHORT												m_usLoadCompletedCnt;

		jh_utility::LockQueue<LogicEventRef>				 m_netLogicEventQueue;
		jh_utility::LockQueue<SessionConnectionEventRef>	m_sessionConnEventQueue;
		jh_utility::LockQueue<GameLanEventPtr>				m_gameLanRequestQueue;
		
		jh::UniquePtr<class jh::GameWorld>					m_pGameWorld;
		jh::UniquePtr<class jh::UserManager>				m_pUserManager;

		std::unordered_map<USHORT, PacketFunc>				m_packetFuncDic;
		
		jh::IocpServer										* m_pOwner;
		HANDLE												m_hLogicThread;
	};
}

