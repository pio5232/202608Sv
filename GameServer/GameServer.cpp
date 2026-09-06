#include "pch.h"
#include <chrono>
#include <random>
#include <WS2tcpip.h>
#include "GameServer.h"
#include "Memory.h"
#include "Job.h"
#include "GameSystem.h"
#include "GameLanClient.h"

jh::GameServer::GameServer() : jh::IocpServer(GAME_SERVER_SAVE_FILE_NAME)
{
	jh_utility::Parser parser;

	const ServerConfig* gameCfgPtr = GetConfig();

	bool succeeded = const_cast<ServerConfig*>(gameCfgPtr)->Read(parser, UP_DIR(GAME_SERVER_CONFIG_FILE), GAME_CATEGORY_NAME);
	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[GameServer] Parse complete. File : [%s]", GAME_SERVER_CONFIG_FILE);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[GameServer] Parse failed.");
		jh_utility::CrashDump::Crash();
	}

	m_pGameSystem = jh::MakeUnique<jh::GameSystem>(this);

	// LanClient 생성
	m_pGameLanClient = jh::MakeUnique<jh::GameLanClient>();

	m_pGameLanClient->SetGameSystem(m_pGameSystem.get());
}


bool jh::GameServer::OnConnectionRequest(const SOCKADDR_IN& clientInfo)
{
	return true;
}



void jh::GameServer::OnError(int errCode, WCHAR* cause)
{
}

void jh::GameServer::OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type)
{
	LogicEventRef logicEvent = jh::MakeShared<jh::LogicEvent>(sessionId, type, packet);

	m_pGameSystem->EnqueueLogicEvent(logicEvent);
}

void jh::GameServer::OnConnected(ULONGLONG sessionId)
{
	SessionConnectionEventRef sessionConnectionEvent = jh::MakeShared<jh::SessionConnectionEvent>(sessionId, jh::SessionConnectionEventType::CONNECT);

	m_pGameSystem->EnqueueSessionConnEvent(sessionConnectionEvent);
}

void jh::GameServer::OnDisconnected(ULONGLONG sessionId)
{
	SessionConnectionEventRef sessionConnectionEvent = jh::MakeShared<jh::SessionConnectionEvent>(sessionId, jh::SessionConnectionEventType::DISCONNECT);

	m_pGameSystem->EnqueueSessionConnEvent(sessionConnectionEvent);
}

void jh::GameServer::OnStarted()
{
	m_pGameSystem->Init();

	m_pGameLanClient->Start();
}

void jh::GameServer::OnStop()
{
	m_pGameSystem->Stop();

	m_pGameLanClient->Stop();
}

void jh::GameServer::Monitor()
{
	wprintf(L" [Game Server] Sessions : %d\n", GetSessionCount());
}