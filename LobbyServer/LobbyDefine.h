#pragma once

namespace jh
{
	class LobbyLanServer;

	class LobbySystem;
	class LobbyServer;
	class RoomManager;
	class UserManager;
}

#define LOBBY_TIMEOUT_CHECK_INTERVAL 5000 // 체크 주기 5초

#define LOBBY_CATEGORY_NAME L"LobbyServer"
#define LOBBY_DATA_CATEGORY_NAME L"LobbyData"
#define LAN_CATEGORY_NAME L"LanServer"

#define USER_MANAGER_SAVE_FILE_NAME L"UserManager"

#define	LOBBY_DATA_CONFIG_FILE L"LobbyData.cfg"
#define LOBBY_SERVER_CONFIG_FILE L"LobbyServer.cfg"

#define LAN_DATA_CONFIG_FILE L"LanData.cfg"
#define LAN_SERVER_CONFIG_FILE L"LanServer.cfg"

#define LOBBY_SERVER_SAVE_FILE_NAME L"LobbyServer"
#define LOBBY_SYSTEM_SAVE_FILE_NAME L"LobbySystem"

#define LAN_SAVE_FILE_NAME L"LobbyLanServer"
#define LOBBY_LAN_SAVE_FILE_NAME L"LobbyLanSystem"

#define ROOM_MANAGER_SAVE_FILE_NAME L"RoomManager"
#define USER_MANAGER_SAVE_FILE_NAME L"UserManager"

#ifdef _DEBUG
	#define MODE L"Debug"
#else
	#define MODE L"Release"
#endif // DEBUG	

//#define TEST

#ifdef TEST
#define GAME_FILE_PATH L"..\\Exe\\" MODE "\\GameServer.exe"
#define GAME_CUR_DIRECTORY L"..\\Exe\\" MODE
#else
#define GAME_FILE_PATH L".\\GameServer.exe"
#define GAME_CUR_DIRECTORY L".\\"
#endif
enum class LanRequestMsgType : byte
{
	NONE = 0,
	GAME_SETTING_REQUEST,
	LAN_INFO_NOTIFY
};

struct LanRequest
{
	explicit LanRequest(ULONGLONG lanSessionId, USHORT msgType, PacketBufferRef packet, jh::IocpServer* lanServer) : m_ullSessionId(lanSessionId), m_usMsgType(msgType), m_pPacket(packet), m_pLanServer(lanServer) {}
	~LanRequest()
	{
		m_ullSessionId = INVALID_SESSION_ID;
		m_usMsgType = jh::INVALID_PACKET;
		m_pPacket.reset();
		m_pLanServer = nullptr;
	}

	LanRequest(const LanRequest& other)				= default;
	LanRequest(LanRequest&& other)					= default;

	LanRequest& operator=(const LanRequest& other)	= default;
	LanRequest& operator=(LanRequest&& other)		= default;

	ULONGLONG				m_ullSessionId;
	USHORT					m_usMsgType;
	PacketBufferRef			m_pPacket;
	jh::IocpServer	* m_pLanServer;
};

using UserRef = std::shared_ptr<class jh::User>;
using RoomRef = std::shared_ptr<class jh::Room>;


struct LobbyServerConfig : MultiServerConfig
{
	~LobbyServerConfig() override = default;

	USHORT m_usMaxRoomCnt;
	USHORT m_usMaxRoomUserCnt;

protected:
	bool GetMainContent(jh_utility::Parser& parser, const WCHAR* categoryName) override;
	bool GetContents(jh_utility::Parser& parser, const WCHAR* categoryName) override;

};
