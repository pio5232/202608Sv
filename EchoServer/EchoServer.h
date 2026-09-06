#pragma once
#include "MultiServer.h"

namespace jh
{
	class EchoServer : public jh::MultiIocpServer
	{
	public:
		EchoServer();
		~EchoServer() override = default;

		void Monitor();
	public:
		void OnRecv(ULONGLONG sessionId, PacketBufferRef packet,USHORT type) override;
		void OnConnected(ULONGLONG sessionId) override;
		void OnDisconnected(ULONGLONG sessionId) override;

		bool OnConnectionRequest(const SOCKADDR_IN& clientInfo) override;
		void OnError(int errCode, WCHAR* cause) override;
	private:
		void OnStarted() override;
		void OnStop() override;

	private:
		//jh::UniquePtr<jh::EchoSystem> m_pEchoSystem;

		// CIocpServer을(를) 통해 상속됨
	};
}

