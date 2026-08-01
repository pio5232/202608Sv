#pragma once

namespace jh
{
	class LobbySystem;

	class LobbyLanSystem
	{
	public:
		using PacketFunc = void(LobbyLanSystem::*)(ULONGLONG, PacketBufferRef&);

	public:
		LobbyLanSystem(jh::IocpServer* owner);
		~LobbyLanSystem();
		
		void Init();
		void SetLobbySystem(class jh::LobbySystem* lobbySystem) { m_pLobbySystem = lobbySystem; }

		void HandleLanInfoNotifyPacket(ULONGLONG sessionId, PacketBufferRef& packet);
		void HandleGameSettingRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet);

		void ProcessPacket(ULONGLONG sessionId, USHORT packetType, PacketBufferRef& packet);

	private:
		jh::IocpServer					* m_pOwner;
		
		std::unordered_map<USHORT, PacketFunc>	m_packetFuncDic;
		class jh::LobbySystem			* m_pLobbySystem;
	};
}

