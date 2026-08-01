#include "pch.h"
#include "LobbyDummyClient.h"
#include "DummyPacketBuilder.h"
#include "DummyUpdateSystem.h"
#include "Memory.h"
jh::LobbyDummyClient::LobbyDummyClient() : IocpClient(L"LobbyDummy")
{
	jh_utility::Parser parser;

	parser.LoadFile(UP_DIR(LOBBY_DUMMY_FILE_NAME));
	parser.SetReadingCategory(LOBBY_DUMMY_CATEGORY_NAME);

	WCHAR ip[IP_STRING_LEN];
	USHORT port;
	USHORT maxSessionCnt;
	DWORD concurrentWorkerThreadCount;

	USHORT lingerOnOff;
	USHORT lingerTime;
	ULONGLONG timeOut;

	bool succeeded = parser.GetValueWstr(L"serverIp", ip, ARRAY_SIZE(ip));
	succeeded &= parser.GetValue(L"serverPort", port);
	succeeded &= parser.GetValue(L"maxSessionCount", maxSessionCnt);
	succeeded &= parser.GetValue(L"concurrentWorkerThreadCount", concurrentWorkerThreadCount);

	succeeded &= parser.GetValue(L"lingerOnOff", lingerOnOff);
	succeeded &= parser.GetValue(L"lingerTime", lingerTime);
	succeeded &= parser.GetValue(L"TimeOut", timeOut);

	parser.CloseFile();

	if (true == succeeded)
		_LOG(L"ParseInfo", LOG_LEVEL_INFO, L"[LobbyDummyClient] Parse success : [%s]", LOBBY_DUMMY_FILE_NAME);
	else
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyDummyClient] Parse failed : [%s]", LOBBY_DUMMY_FILE_NAME);
		jh_utility::CrashDump::Crash();
	}

	InitClientConfig(ip, port, concurrentWorkerThreadCount, lingerOnOff, lingerTime, timeOut);

	if (false == InitSessionArray(maxSessionCnt))
	{
		_LOG(L"ParseInfo", LOG_LEVEL_WARNING, L"[LobbyDummyClient] InitSessionArray failed.");
		jh_utility::CrashDump::Crash();
	}

	m_pDummySystem = jh::MakeUnique<jh::DummyUpdateSystem>(this);

}

jh::LobbyDummyClient::~LobbyDummyClient()
{
}

void jh::LobbyDummyClient::OnRecv(ULONGLONG sessionId, PacketBufferRef packet, USHORT type)
{
	int threadNum = static_cast<int>(sessionId) % LOGIC_THREAD_COUNT;

	DummyThreadJobRef job = jh::MakeShared<DummyThreadJob>(sessionId, type, packet); //MakeJob(sessionId, type, packet);

	m_pDummySystem->EnqueueLogicEvent(job, threadNum);
}

void jh::LobbyDummyClient::OnConnected(ULONGLONG sessionId)
{
	int threadNum = static_cast<int>(sessionId) % LOGIC_THREAD_COUNT;

	SessionConnectionEventRef sessionConnEvent = jh::MakeShared<SessionConnectionEvent>(sessionId, SessionConnectionEventType::CONNECT);// MakeSystemJob(sessionId, jh_utility::SessionConnectionEventType::CONNECT);

	m_pDummySystem->EnqueueSessionConnEvent(sessionConnEvent, threadNum);
}

void jh::LobbyDummyClient::OnDisconnected(ULONGLONG sessionId)
{
	int threadNum = static_cast<int>(sessionId) % LOGIC_THREAD_COUNT;

	SessionConnectionEventRef sessionConnEvent = jh::MakeShared<SessionConnectionEvent>(sessionId, SessionConnectionEventType::DISCONNECT);// MakeSystemJob(sessionId, jh_utility::SessionConnectionEventType::CONNECT);

	m_pDummySystem->EnqueueSessionConnEvent(sessionConnEvent, threadNum);
}

void jh::LobbyDummyClient::Monitor()
{
	wprintf(L"=================================================\n");

	wprintf(L" [Network] Send TPS : %ld\n", GetSendCount());
	wprintf(L" [Network] Recv TPS : %ld\n", GetRecvCount()); ;

	wprintf(L" [Network] Disconnected Session Count : %lld\n", GetDisconnectedCount());
	wprintf(L" [Network] Total Disconnected Session Count : %lld\n", GetTotalDisconnectedCount());
	wprintf(L"=================================================\n");

	wprintf(L" [Content] MAX Sessions : %d\n", GetMaxSessionCount());
	wprintf(L" [Content] Total Sessions : %d\n", GetSessionCount());
	wprintf(L" [Content] Total Dummies : %d\n", DummyData::aliveDummyCount);

	DummyUpdateSystem::EtcData etcData = m_pDummySystem->UpdateEtc();

	wprintf(L" [Content] total Dummy Disconnect_Count .. : [%d]\n", etcData.m_lDummyDisconnectCount);
	wprintf(L" [Content] Packet Re-Send Timeout Count [%d]s : %d\n", RE_SEND_TIMEOUT / 1000, etcData.m_lReSendTimeoutCount);

	wprintf(L" [Content] Enter Failed.. total DestroyedRoom_Count : [%d]\n", etcData.m_lDestroyedRoomErrorCount);
	wprintf(L" [Content] Enter Failed.. total DiffRoom_Count : [%d]\n", etcData.m_lDiffRoomNameErrorCount);
	wprintf(L" [Content] Enter Failed.. total FullRoom_Count : [%d]\n", etcData.m_lFullRoomErrorCount);
	wprintf(L" [Content] Enter Failed.. total AlreadyRunning_Count : [%d]\n", etcData.m_lAlreadyRunningRoomErrorCount);

	// RoomListRequestPacket
	// Chat
	// MakeRoom
	// EnterRoom
	// LeaveRoom
	// Login

	wprintf(L" [Content] Wait [Login]			Packet count : %ld \n", etcData.m_pendingDataForPrint.m_lLoginPacket);
	wprintf(L" [Content] Wait [Chat]			Packet count : %ld \n", etcData.m_pendingDataForPrint.m_lChatPacket);
	wprintf(L" [Content] Wait [MakeRoom]		Packet count : %ld \n", etcData.m_pendingDataForPrint.m_lMakeRoomPacket);
	wprintf(L" [Content] Wait [EnterRoom]		Packet count : %ld  \n", etcData.m_pendingDataForPrint.m_lEnterRoomPacket);
	wprintf(L" [Content] Wait [LeaveRoom]		Packet count : %ld \n", etcData.m_pendingDataForPrint.m_lLeaveRoomPacket);
	wprintf(L" [Content] Wait [RoomListRequestPacket]	Packet count : %ld \n", etcData.m_pendingDataForPrint.m_lRoomListPacket);

	wprintf(L"[RTT] : [%llu]ms \n", m_pDummySystem->GetRTT());
}
void jh::LobbyDummyClient::OnStart()
{
	m_pDummySystem->Init();
}

void jh::LobbyDummyClient::OnStop()
{
	m_pDummySystem->Stop();
}

