#pragma once
#include "NetworkBase.h"
#include "NetworkUtils.h"

namespace jh
{
	class LobbyLanSystem;
	class LobbyLanServer : public jh::IocpServer
	{
	public:
		LobbyLanServer();
		~LobbyLanServer();

		virtual bool OnConnectionRequest(const SOCKADDR_IN& clientInfo);
		virtual void OnError(int errCode, WCHAR* cause);

		void OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type) override;
		void OnConnected(ULONGLONG sessionId) override;
		void OnDisconnected(ULONGLONG sessionId) override;

		void Init();
		void OnStop() override;

		void SetLobbySystem(class jh::LobbySystem* lobbySystem);
	private:
		jh::UniquePtr <jh::LobbyLanSystem> m_pLobbyLanSystem;
	};
}