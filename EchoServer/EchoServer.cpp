#include "pch.h"
#include "EchoServer.h"
#include "EchoSystem.h"
#include "Memory.h"

jh::EchoServer::EchoServer() : MultiIocpServer(ECHO_SERVER_SAVE_FILE_NAME)
{
	jh_utility::Parser parser;

	const MultiServerConfig* echoCfg = static_cast<const MultiServerConfig*>(GetConfig());
	
	bool succeeded = const_cast<MultiServerConfig*>(echoCfg)->Read(parser, UP_DIR(ECHO_SERVER_CONFIG_FILE), ECHO_CATEGORY_NAME);

	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[LobbyLanServer] Parse success: [%s]", ECHO_SERVER_CONFIG_FILE);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] Parse failed: [%s]", ECHO_SERVER_CONFIG_FILE);
		jh_utility::CrashDump::Crash();
	}


	if (false == InitSessionArray(echoCfg->m_dwMaxSessionCnt))
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] InitSessionArray failed.");
		jh_utility::CrashDump::Crash();
	}

	//m_pEchoSystem = jh::MakeUnique<EchoSystem>(this);
	//m_pEchoSystem->Init();
}

void jh::EchoServer::Monitor()
{
	wprintf(L" [Echo Server] Sessions : %d\n", GetSessionCount());
}

void jh::EchoServer::OnStarted()
{
}

void jh::EchoServer::OnStop()
{
}
//void jh::EchoServer::OnRecv(LONGLONG sessionId, jh_utility::SerializationBuffer* packet, WORD type)

void jh::EchoServer::OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type)
{
	// 멀티 스레드 처리 로직
	short len;
	ULONGLONG data;

	*packet >> data;

	PacketBufferRef sendPacket = jh::PacketBuilder::BuildEchoPacket(8, data);
	SendPacket(sessionId, sendPacket);
	return;
	

	// 단일 스레드 처리 로직.
	//JobRef job = std::make_shared<jh_utility::Job>(sessionId, type, packet);
	//JobRef job = jh::MakeShared<jh_utility::Job>(sessionId, type, packet);


	//m_pEchoSystem->EnqueueJob(job);
}

void jh::EchoServer::OnConnected(ULONGLONG sessionId)
{
	//SessionConnectionEventRef systemJob = std::make_shared<jh_utility::SessionConnectionEvent>(sessionId, jh_utility::SessionConnectionEventType::CONNECT);
	//SessionConnectionEventRef sessionConnectionEventPtr = MakeShared<jh_utility::SessionConnectionEvent>(g_pMemSystem, sessionId, jh_utility::SessionConnectionEventType::CONNECT);

	//m_pEchoSystem->EnqueueSystemJob(sessionConnectionEventPtr);
}
void jh::EchoServer::OnDisconnected(ULONGLONG sessionId)
{
	//SessionConnectionEventRef systemJob = std::make_shared<jh_utility::SessionConnectionEvent>(sessionId, jh_utility::SessionConnectionEventType::DISCONNECT);
	//SessionConnectionEventRef sessionConnectionEventPtr = MakeShared<jh_utility::SessionConnectionEvent>(g_pMemSystem, sessionId, jh_utility::SessionConnectionEventType::DISCONNECT); // MakeSystemJob(sessionId, jh_utility::SessionConnectionEventType::DISCONNECT);


	//m_pEchoSystem->EnqueueSystemJob(sessionConnectionEventPtr);
}

bool EchoServer::OnConnectionRequest(const SOCKADDR_IN& clientInfo)
{
	return true;
}

void EchoServer::OnError(int errCode, WCHAR* cause)
{
}
