#include "pch.h"
#include "LanServer.h"
#include "LobbyLanSystem.h"
#include "LobbySystem.h"
#include "Memory.h"

jh::LobbyLanServer::LobbyLanServer() : jh::MultiIocpServer(LAN_SAVE_FILE_NAME), m_pLobbyLanSystem(nullptr)
{
	jh_utility::Parser parser;

	const ServerConfig* cfgPtr = GetConfig();

	bool succeeded = const_cast<ServerConfig*>(cfgPtr)->Read(parser, UP_DIR(LAN_SERVER_CONFIG_FILE), LAN_CATEGORY_NAME);

	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[LobbyLanServer] Parse success: [%s]", LOBBY_SERVER_CONFIG_FILE);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] Parse failed: [%s]", LOBBY_SERVER_CONFIG_FILE);
		jh_utility::CrashDump::Crash();
	}

	
	if (false == InitSessionArray(cfgPtr->m_dwMaxSessionCnt))
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] InitSessionArray failed.");	
		jh_utility::CrashDump::Crash();
	}

	m_pLobbyLanSystem = jh::MakeUnique<jh::LobbyLanSystem>(this);
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


void jh::LobbyLanServer::SetLobbySystem(jh::LobbySystem* lobbySystem)
{
	m_pLobbyLanSystem->SetLobbySystem(lobbySystem);
}