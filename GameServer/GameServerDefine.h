#pragma once

namespace jh
{
	class GameSession;
	class LanClientSession;
}


#define GAME_SERVER_CONFIG_FILE L"GameServer.cfg"
#define GAME_LAN_CLIENT_CONFIG_FILE L"GameLanClient.cfg"

#define GAME_CATEGORY_NAME L"GameServer"
#define GAME_LAN_CATEGORY_NAME L"GameLanClient"

#define GAME_SERVER_SAVE_FILE_NAME L"GameServer"
#define GAME_SYSTEM_SAVE_FILE_NAME L"GameSystem"
#define GAME_LAN_CLIENT_SAVE_FILE_NAME L"GameLanClient"

#define GAME_USER_MANAGER_SAVE_FILE_NAME L"UserManager"

using SendPacketFunc		= std::function<void(ULONGLONG, PacketBufferRef&)>; // [sessionId, packet]
using GameSessionPtr		= std::shared_ptr<jh::GameSession>;
using LanClientSessionPtr	= std::shared_ptr<jh::LanClientSession>;

constexpr int fixedFrame			= 60;
constexpr float fixedDeltaTime		= 1.0f / fixedFrame;

constexpr float limitDeltaTime		= 0.2f;

constexpr float VictoryZoneMinX		= 45.0f;
constexpr float VictoryZoneMaxX		= 55.0f;

constexpr float VictoryZoneMinZ		= 45.0f;
constexpr float VictoryZoneMaxZ		= 55.0f;

constexpr float COS_30				= 0.8660254f;

constexpr float mapXMin				= 0;
constexpr float mapXMax				= 100.0f;
constexpr float mapZMin				= 0;
constexpr float mapZMax				= 100.0f;

constexpr float centerX				= (mapXMax - mapXMin) / 2.0f;
constexpr float centerZ				= (mapZMax - mapZMin) / 2.0f;
constexpr int sectorCriteriaSize	= 10;

constexpr int sectorMaxX			= ((int)mapXMax - (int)mapXMin) / sectorCriteriaSize + 2; // 0 [ 1 2 3 4 5 ] 6
constexpr int sectorMaxZ			= ((int)mapZMax - (int)mapZMin) / sectorCriteriaSize + 2;

// for문 사용
// for( i = startX/Z; i<endX/Z; i++)
constexpr int startXSectorPos					= 1;
constexpr int startZSectorPos					= 1;
constexpr int endXSectorPos						= sectorMaxX - 1;
constexpr int endZSectorPos						= sectorMaxZ - 1;

constexpr float edgeThreshold					= 10.0f;

constexpr float defaultSlowWalkSpeed			= 3.0f;
constexpr float defaultWalkSpeed				= 6.0f;
constexpr float defaultRunSpeed					= 9.0f;

constexpr USHORT defaultMaxHp					= 3;
constexpr USHORT defaultAttackDamage			= 1;

constexpr float defaultAttackRange				= 1.6f;

constexpr float defaultErrorRange				= 0.5f; // 오차 범위
constexpr float posUpdateInterval				= 0.2f;

constexpr float attackDuration					= 0.8f;
constexpr float attackedDuration				= 1.0f;
constexpr float DeadDuration					= 3.0f;

constexpr ULONGLONG victoryZoneCheckDuration	= 10000; // 이름 바꾸기. 존 진입 시 우승자가 되기 위해 견뎌야하는 시간 (ms)

namespace jh
{
	class Entity;
	class GamePlayer;
	class AIPlayer;
	class WorldChat;
}


Vector3 GenerateRandomPos();

namespace jh
{
	enum class GameLanRequestMsgType
	{
		NONE = 0,
	};

	struct GameLanEvent
	{
		explicit GameLanEvent(ULONGLONG lanSessionId, USHORT msgType, PacketBufferRef packet, jh::IocpClient* lanClient) : m_ullSessionId(lanSessionId), m_usMsgType(msgType), m_pPacket(packet), m_pClient(lanClient) {}
		~GameLanEvent()
		{
			m_ullSessionId = INVALID_SESSION_ID;
			m_usMsgType = jh::INVALID_PACKET;
			m_pPacket.reset();
			m_pClient = nullptr;
		}

		GameLanEvent(const GameLanEvent& other) = default;
		GameLanEvent(GameLanEvent&& other) = default;

		GameLanEvent& operator=(const GameLanEvent& other) = default;
		GameLanEvent& operator=(GameLanEvent&& other) = default;

		ULONGLONG				m_ullSessionId;
		USHORT					m_usMsgType;
		PacketBufferRef			m_pPacket;
		jh::IocpClient* m_pClient;
	};

	struct LogicEvent
	{
		LogicEvent(ULONGLONG id, USHORT type, PacketBufferRef packet) :
			m_llSessionId(id), m_usPktType(type), m_pPacket(packet) {}

		ULONGLONG	m_llSessionId;
		USHORT		m_usPktType;
		PacketBufferRef	m_pPacket;

		~LogicEvent()
		{
			m_llSessionId = INVALID_SESSION_ID;
			m_usPktType = (USHORT)(jh::INVALID_PACKET);
			m_pPacket.reset();
		}

		LogicEvent(const LogicEvent& other) = default;
		LogicEvent(LogicEvent&& other) = default;

		LogicEvent& operator=(const LogicEvent& other) = default;
		LogicEvent& operator=(LogicEvent&& other) = default;
	};


	enum class SessionConnectionEventType : byte
	{
		NONE = 0,
		CONNECT = 1,
		DISCONNECT,
	};

	/// <summary>
	/// Session의 연결 / 해제를 알리는 용도로 사용된다.
	/// </summary>
	struct SessionConnectionEvent
	{
		SessionConnectionEvent(ULONGLONG id, jh::SessionConnectionEventType msg) : m_ullSessionId(id), m_eventType(msg) {}
		~SessionConnectionEvent()
		{
			m_ullSessionId = INVALID_SESSION_ID;
			m_eventType = SessionConnectionEventType::NONE;
		}

		SessionConnectionEvent(const SessionConnectionEvent& other) = default;
		SessionConnectionEvent(SessionConnectionEvent&& other) = default;

		SessionConnectionEvent& operator=(const SessionConnectionEvent& other) = default;
		SessionConnectionEvent& operator=(SessionConnectionEvent&& other) = default;

		ULONGLONG					m_ullSessionId;
		SessionConnectionEventType	m_eventType;
	};
}

struct GameLanEvent
{
	explicit GameLanEvent(ULONGLONG lanSessionId, USHORT msgType, PacketBufferRef packet, jh::IocpClient* lanClient) : m_ullSessionId(lanSessionId), m_usMsgType(msgType), m_pPacket(packet), m_pClient(lanClient) {}
	~GameLanEvent()
	{
		m_ullSessionId = INVALID_SESSION_ID;
		m_usMsgType = jh::INVALID_PACKET;
		m_pPacket.reset();
		m_pClient = nullptr;
	}

	GameLanEvent(const GameLanEvent& other) = default;
	GameLanEvent(GameLanEvent&& other) = default;

	GameLanEvent& operator=(const GameLanEvent& other) = default;
	GameLanEvent& operator=(GameLanEvent&& other) = default;

	ULONGLONG				m_ullSessionId;
	USHORT					m_usMsgType;
	PacketBufferRef			m_pPacket;
	jh::IocpClient	* m_pClient;
};


using UserRef = std::shared_ptr<class jh::User>;

using GamePlayerPtr = std::shared_ptr<class jh::GamePlayer>;
using AIPlayerPtr = std::shared_ptr<class jh::AIPlayer>;
using EntityPtr = std::shared_ptr<class jh::Entity>;

using WorldChatPtr = std::shared_ptr<class jh::WorldChat>;
using Action = std::function<void()>;

using GameLanEventPtr = std::shared_ptr<jh::GameLanEvent>;
using LogicEventRef = std::shared_ptr<jh::LogicEvent>;
using SessionConnectionEventRef = std::shared_ptr<jh::SessionConnectionEvent>;

struct TimerAction
{
	ULONGLONG executeTick;
	Action action;

	const bool operator< (const TimerAction& other) const
	{
		return executeTick > other.executeTick;
	}
};
