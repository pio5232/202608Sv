#include "pch.h"
#include "LobbyServer.h"
#include "LanServer.h"
#include "LobbySystem.h"
#include "Job.h"
#include "Memory.h"


jh::LobbyServer::LobbyServer() : IocpServer{ LOBBY_SERVER_SAVE_FILE_NAME }/*, _canCheckHeartbeat(true)*/
{
	jh_utility::Parser parser;

	parser.LoadFile(UP_DIR(LOBBY_SERVER_CONFIG_FILE));
	parser.SetReadingCategory(LOBBY_CATEGORY_NAME);

	LobbyServerConfig* config = CreateConfig();

	bool succeeded = parser.GetValueWstr(L"serverIp", config.m_wszIp, ARRAY_SIZE(config.m_wszIp));
	succeeded &= parser.GetValue(L"serverPort", config.m_usPort);
	succeeded &= parser.GetValue(L"maxSessionCount", config.m_dwMaxSessionCnt);
	succeeded &= parser.GetValue(L"concurrentWorkerThreadCount", config.m_dwConcurrentWorkerThreadCount);

	succeeded &= parser.GetValue(L"lingerOnOff", config.m_lingerOption.l_onoff);
	succeeded &= parser.GetValue(L"lingerTime", config.m_lingerOption.l_linger);
	succeeded &= parser.GetValue(L"Timeout", config.m_ullTimeoutLimit);
	succeeded &= parser.GetValue(L"TimeoutCheckInterval", config.m_ullTimeoutCheckInterval);
	succeeded &= parser.GetValue(L"WorkerTick", config.m_ullWorkerTick);

	parser.SetReadingCategory(LOBBY_DATA_CATEGORY_NAME);

	succeeded &= parser.GetValue(L"maxRoomCount", config.m_usMaxRoomCnt);
	succeeded &= parser.GetValue(L"maxRoomUserCount", config.m_usMaxRoomUserCnt);

	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[LobbyServer] Parsing LobbyServer complete. File: [%s]", LOBBY_SERVER_CONFIG_FILE);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyServer] Parsing LobbyServer failed.");
		jh_utility::CrashDump::Crash();
	}

	LoadConfig(config);
	
	if (false == InitSessionArray(config.m_dwMaxSessionCnt))
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyServer] InitSessionArray failed.");		
		jh_utility::CrashDump::Crash();
	}

	m_pLanServer = jh::MakeUnique<jh::LobbyLanServer>();
	m_pLobbySystem = jh::MakeUnique<jh::LobbySystem>(this, config.m_usMaxRoomCnt, config.m_usMaxRoomUserCnt);

	m_pLobbySystem->Init();
	
	m_pLanServer->SetLobbySystem(m_pLobbySystem.get());
	m_pLanServer->Init();
	
}

jh::LobbyServer::~LobbyServer()
{

}

void jh::LobbyServer::OnStart()
{
	m_pLanServer->Start();
}

void jh::LobbyServer::OnStop()
{
	m_pLobbySystem->Stop();

	m_pLanServer->Stop();
	
}
void jh::LobbyServer::OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type)
{
	m_pLobbySystem->ProcessPacket(sessionId, type, packet);
}
void jh::LobbyServer::OnConnected(ULONGLONG sessionId)
{
}

void jh::LobbyServer::OnDisconnected(ULONGLONG sessionId)
{
	m_pLobbySystem->DisconnectUser(sessionId);
}

void jh::LobbyServer::OnWorkerThreadUpdate()
{
	jh_utility::ThreadExecutor::DistributeReservedJobs();

	jh_utility::ThreadExecutor::DoGlobalQueueWork();
}

void jh::LobbyServer::Monitor()
{
	wprintf(L" [Network] Send TPS : %ld\n", GetSendCount());
	wprintf(L" [Network] Recv TPS : %ld\n", GetRecvCount());

	wprintf(L" [Network] Disconnected Session Count : %lld\n", GetDisconnectedCount());
	wprintf(L" [Network] Total Disconnected Session Count : %lld\n", GetTotalDisconnectedCount());

	wprintf(L" [Lobby Server] Accepted Count: %lld\n", GetTotalAcceptedCount());
	wprintf(L" [Lobby Server] Sessions Count : %ld\n", GetSessionCount());
	wprintf(L" [LAN Server]   Sessions Count : %ld\n", m_pLanServer->GetSessionCount());
}

void jh::LobbyServer::GetInvalidMsgCnt()
{
	m_pLobbySystem->GetInvalidMsgCnt();
}
