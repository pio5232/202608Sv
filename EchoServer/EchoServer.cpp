#include "pch.h"
#include "EchoServer.h"
#include "EchoSystem.h"
#include "Memory.h"

jh::EchoServer::EchoServer() : IocpServer(ECHO_SERVER_SAVE_FILE_NAME)
{
	jh_utility::Parser parser;

	parser.LoadFile(UP_DIR(ECHO_SERVER_CONFIG_FILE));
	parser.SetReadingCategory(ECHO_CATEGORY_NAME);

	MultiServerConfig* config = CreateConfig();

	bool succeeded = parser.GetValueWstr(L"serverIp", config->m_wszIp, ARRAY_SIZE(config->m_wszIp));
	succeeded &= parser.GetValue(L"serverPort", config->m_usPort);
	succeeded &= parser.GetValue(L"maxSessionCount", config->m_dwMaxSessionCnt);
	succeeded &= parser.GetValue(L"concurrentWorkerThreadCount", config->m_dwConcurrentWorkerThreadCount);

	succeeded &= parser.GetValue(L"lingerOnOff", config->m_lingerOption.l_onoff);
	succeeded &= parser.GetValue(L"lingerTime", config->m_lingerOption.l_linger);
	succeeded &= parser.GetValue(L"Timeout", config->m_ullTimeoutLimit);
	succeeded &= parser.GetValue(L"TimeoutCheckInterval", config->m_ullTimeoutCheckInterval);
	succeeded &= parser.GetValue(L"WorkerTick", config->m_ullWorkerTick);

	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[LobbyLanServer] Parse success: [%s]", ECHO_SERVER_CONFIG_FILE);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] Parse failed: [%s]", ECHO_SERVER_CONFIG_FILE);
		jh_utility::CrashDump::Crash();
	}


	if (false == InitSessionArray(config->m_dwMaxSessionCnt))
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyLanServer] InitSessionArray failed.");
		jh_utility::CrashDump::Crash();
	}

	//m_pEchoSystem = jh::MakeUnique<EchoSystem>(this);
	//m_pEchoSystem->Init();
}

jh::EchoServer::~EchoServer()
{
}

void jh::EchoServer::Monitor()
{
	wprintf(L" [Echo Server] Sessions : %d\n", GetSessionCount());
}

void jh::EchoServer::OnStart()
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
