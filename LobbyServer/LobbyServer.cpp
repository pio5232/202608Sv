#include "pch.h"
#include "LobbyServer.h"
#include "LanServer.h"
#include "LobbySystem.h"
#include "Job.h"
#include "Memory.h"


jh::LobbyServer::LobbyServer() : MultiIocpServer{ LOBBY_SERVER_SAVE_FILE_NAME }/*, _canCheckHeartbeat(true)*/
{
	jh_utility::Parser parser;

	const LobbyServerConfig* lobbyCfgPtr = static_cast<const LobbyServerConfig*>(GetConfig());
	
	bool succeeded = const_cast<LobbyServerConfig*>(lobbyCfgPtr)->Read(parser, UP_DIR(LOBBY_SERVER_CONFIG_FILE), LOBBY_CATEGORY_NAME);
	succeeded &= const_cast<LobbyServerConfig*>(lobbyCfgPtr)->ReadOtherCategory(parser, LOBBY_DATA_CATEGORY_NAME);

	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[LobbyServer] Parsing LobbyServer complete. File: [%s]", LOBBY_SERVER_CONFIG_FILE);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyServer] Parsing LobbyServer failed.");
		jh_utility::CrashDump::Crash();
	}

	if (false == InitSessionArray(lobbyCfgPtr->m_dwMaxSessionCnt))
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyServer] InitSessionArray failed.");		
		jh_utility::CrashDump::Crash();
	}

	m_pLanServer = jh::MakeUnique<jh::LobbyLanServer>();

	auto updateHbFunc = [this](ULONGLONG _sessionId, ULONGLONG _time) {this->UpdateHeartbeat(_sessionId, _time); };

	m_pLobbySystem = jh::MakeUnique<jh::LobbySystem>(this, lobbyCfgPtr->m_usMaxRoomCnt, lobbyCfgPtr->m_usMaxRoomUserCnt, updateHbFunc);

	m_pLobbySystem->Init();
	
	m_pLanServer->SetLobbySystem(m_pLobbySystem.get());
	m_pLanServer->Init();
	
}


void jh::LobbyServer::OnStarted()
{
	m_pLanServer->Start();
}

void jh::LobbyServer::OnStop()
{
	m_pLobbySystem->Stop();

	m_pLanServer->Stop();
	
}

LobbyServerConfig* LobbyServer::CreateConfig() 
{
	return static_cast<LobbyServerConfig*>(g_pMemSystem->Alloc(sizeof(LobbyServerConfig)));
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

bool LobbyServer::OnConnectionRequest(const SOCKADDR_IN& clientInfo)
{
	return true;
}

void LobbyServer::OnError(int errCode, WCHAR* cause)
{
}

void jh::LobbyServer::OnWorkerThreadUpdateEnd()
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

void jh::LobbyServer::GetInvalidMsgCnt() const
{
	m_pLobbySystem->GetInvalidMsgCnt();
}
