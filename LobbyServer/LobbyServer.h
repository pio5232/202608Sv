#pragma once

#include "MultiServer.h"
#include "NetworkBase.h"
#include "NetworkUtils.h"

namespace jh
{
    class LobbyLanServer;
    class LobbySystem;

    class LobbyServer : public jh::MultiIocpServer
    {
    public:
        LobbyServer();
        ~LobbyServer() override = default;
        void OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type) override;
        void OnConnected(ULONGLONG sessionId) override;
        void OnDisconnected(ULONGLONG sessionId) override;

        bool OnConnectionRequest(const SOCKADDR_IN& clientInfo) override;
        void OnError(int errCode, WCHAR* cause) override;

        virtual void OnWorkerThreadUpdateEnd() override;
        void Monitor();
        void GetInvalidMsgCnt() const;
    private:
        void OnStarted() override;
        void OnStop() override;
        virtual LobbyServerConfig* CreateConfig() override;

        jh::UniquePtr<class jh::LobbyLanServer>   m_pLanServer;
        jh::UniquePtr<class jh::LobbySystem>      m_pLobbySystem;

    };
}