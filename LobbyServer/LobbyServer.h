#pragma once

#include "NetworkBase.h"
#include "NetworkUtils.h"

namespace jh
{
    class LobbyLanServer;
    class LobbySystem;

    class LobbyServer : public jh::IocpServer
    {
    public:
        LobbyServer();
        ~LobbyServer();

        virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type) override;
        virtual void OnConnected(ULONGLONG sessionId) override;
        virtual void OnDisconnected(ULONGLONG sessionId) override;

        virtual void OnWorkerThreadUpdate() override;
        void Monitor();
        void GetInvalidMsgCnt();
    private:
        void OnStart() override;
        void OnStop() override;

        jh::UniquePtr<class jh::LobbyLanServer>   m_pLanServer;
        jh::UniquePtr<class jh::LobbySystem>      m_pLobbySystem;

    };
}