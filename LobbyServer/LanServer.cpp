#include "pch.h"
#include "LanServer.h"
#include "LobbyLanSystem.h"
#include "LobbySystem.h"
#include "Memory.h"

jh::LobbyLanServer::LobbyLanServer() : IocpServer(LAN_SAVE_FILE_NAME), m_pLobbyLanSystem(nullptr)
{
	jh_utility::Parser parser;
	
	parser.LoadFile(UP_DIR(LAN_SERVER_CONFIG_FILE));
	parser.SetReadingCategory(LAN_CATEGORY_NAME);

	ServerConfig config;

	bool succeeded = parser.GetValueWstr(L"serverIp", config.m_wszIp, ARRAY_SIZE(config.m_wszIp));
	succeeded &= parser.GetValue(L"serverPort", config.m_usPort);
	succeeded &= parser.GetValue(L"maxSessionCount", config.m_dwMaxSessionCnt);
	succeeded &= parser.GetValue(L"concurrentWorkerThreadCount", config.m_dwConcurrentWorkerThreadCount);

	succeeded &= parser.GetValue(L"lingerOnOff", config.m_lingerOption.l_onoff);
	succeeded &= parser.GetValue(L"lingerTime", config.m_lingerOption.l_linger);
	succeeded &= parser.GetValue(L"Timeout", config.m_ullTimeoutLimit);
	succeeded &= parser.GetValue(L"TimeoutCheckInterval", config.m_ullTimeoutCheckInterval);
	succeeded &= parser.GetValue(L"WorkerTick", config.m_ullWorkerTick);
	
	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[LobbyLanServer] Parse success: [%s]", LOBBY_SERVER_CONFIG_FILE);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] Parse failed: [%s]", LOBBY_SERVER_CONFIG_FILE);
		jh_utility::CrashDump::Crash();
	}

	LoadConfig(config);
	
	if (false == InitSessionArray(config.m_dwMaxSessionCnt))
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] InitSessionArray failed.");	
		jh_utility::CrashDump::Crash();
	}

	m_pLobbyLanSystem = jh::MakeUnique<jh::LobbyLanSystem>(this);
}

jh::LobbyLanServer::~LobbyLanServer()
{

}

bool jh::LobbyLanServer::OnConnectionRequest(const SOCKADDR_IN& clientInfo)
{
	return true;
}

void jh::LobbyLanServer::OnError(int errCode, WCHAR* cause)
{
}


void jh::LobbyLanServer::OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type)
{
	m_pLobbyLanSystem->ProcessPacket(sessionId, type, packet);
}
void jh::LobbyLanServer::OnConnected(ULONGLONG sessionId)
{

}
void jh::LobbyLanServer::OnDisconnected(ULONGLONG sessionId)
{

}

void jh::LobbyLanServer::Init()
{
	m_pLobbyLanSystem->Init();
}

void jh::LobbyLanServer::OnStop()
{
}

void jh::LobbyLanServer::SetLobbySystem(jh::LobbySystem* lobbySystem)
{
	m_pLobbyLanSystem->SetLobbySystem(lobbySystem);
}