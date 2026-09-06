#pragma once
#include <atomic>
#include "Utils.h"

namespace jh
{
	class GameSystem;
	class GameLanClient;

	class GameWorld;
	class UserManager;

	class GameServer : public jh::IocpServer
	{
	public:
		
		GameServer();

		~GameServer() override = default;

		bool OnConnectionRequest (const SOCKADDR_IN& clientInfo) override;
		void OnError(int errCode, WCHAR* cause) override;

		void OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type) override;
		void OnConnected(ULONGLONG sessionId) override;
		void OnDisconnected(ULONGLONG sessionId) override;

		void OnStarted() override;
		void OnStop() override;

		void Monitor();
	private:
		jh::UniquePtr<class jh::GameSystem>		m_pGameSystem;
		jh::UniquePtr<class jh::GameLanClient>	m_pGameLanClient;
	};

}