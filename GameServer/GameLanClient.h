#pragma once

namespace jh
{
	class GameSystem;

	class GameLanClient : public jh::IocpClient
	{
	public:
		GameLanClient();
		~GameLanClient();

		void SetGameSystem(class GameSystem* gameSystem) { m_pGameSystem = gameSystem; }
		
		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type) override;

		virtual void OnConnected(ULONGLONG sessionId) override;
		virtual void OnDisconnected(ULONGLONG sessionId) override;

	private:
		class GameSystem* m_pGameSystem;
	};
}
