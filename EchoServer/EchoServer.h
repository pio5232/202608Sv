#pragma once

namespace jh
{
	class EchoServer : public jh::IocpServer
	{
	public:
		EchoServer();
		~EchoServer();

		void Monitor();
	public:
		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef packet,USHORT type) override;
		void OnConnected(ULONGLONG sessionId) override;
		void OnDisconnected(ULONGLONG sessionId) override;

	private:
		void OnStart() override;
		void OnStop() override;

	private:
		//jh::UniquePtr<jh::EchoSystem> m_pEchoSystem;

		// CIocpServer을(를) 통해 상속됨
	};
}

