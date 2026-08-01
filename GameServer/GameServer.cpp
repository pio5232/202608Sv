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

	parser.LoadFile(UP_DIR(GAME_SERVER_CONFIG_FILE));
	parser.SetReadingCategory(GAME_CATEGORY_NAME);

	ServerConfig* gameServerConfig = CreateConfig();

	bool succeeded = parser.GetValueWstr(L"serverIp", gameServerConfig->m_wszIp, ARRAY_SIZE(gameServerConfig->m_wszIp));
	succeeded &= parser.GetValue(L"serverPort", gameServerConfig->m_usPort);
	succeeded &= parser.GetValue(L"maxSessionCount", gameServerConfig->m_dwMaxSessionCnt);
	succeeded &= parser.GetValue(L"concurrentWorkerThreadCount", gameServerConfig->m_dwConcurrentWorkerThreadCount);

	succeeded &= parser.GetValue(L"lingerOnOff", gameServerConfig->m_lingerOption.l_onoff);
	succeeded &= parser.GetValue(L"lingerTime", gameServerConfig->m_lingerOption.l_linger);
	succeeded &= parser.GetValue(L"TimeOut", gameServerConfig->m_ullTimeoutLimit);
	succeeded &= parser.GetValue(L"TimeoutCheckInterval", gameServerConfig->m_ullTimeoutCheckInterval);

	//succeeded &= parser.GetValue(L"WorkerTick", gameServerConfig->m_ullWorkerTick);

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

jh::GameServer::~GameServer()
{
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

void jh::GameServer::OnStart()
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