#pragma once

namespace jh
{
	class DummyUpdateSystem;
	class LobbyDummyClient : public jh::IocpClient
	{
	public:
		LobbyDummyClient();
		virtual ~LobbyDummyClient();

		// IocpClient을(를) 통해 상속됨
		void OnRecv(ULONGLONG sessionId, PacketBufferRef dataBuffer, USHORT type) override;

		void OnConnected(ULONGLONG sessionId) override;
		void OnDisconnected(ULONGLONG sessionId) override;

		void Monitor();
		jh::UniquePtr<jh::DummyUpdateSystem> m_pDummySystem;

		virtual void OnStart() override;
		virtual void OnStop() override;

	};
}

