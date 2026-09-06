#pragma once
#include "MultiServer.h"
#include "NetworkBase.h"
#include "NetworkUtils.h"

namespace jh
{
	class LobbyLanSystem;
	class LobbyLanServer : public jh::MultiIocpServer
	{
	public:
		LobbyLanServer();
		~LobbyLanServer() override = default;

		bool OnConnectionRequest(const SOCKADDR_IN& clientInfo) override;
		void OnError(int errCode, WCHAR* cause) override;

		void OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type) override;
		void OnConnected(ULONGLONG sessionId) override;
		void OnDisconnected(ULONGLONG sessionId) override;

		void Init();

		void SetLobbySystem(class jh::LobbySystem* lobbySystem);
	private:
		jh::UniquePtr <jh::LobbyLanSystem> m_pLobbyLanSystem;
	};
}