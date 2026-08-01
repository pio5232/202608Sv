#pragma once

namespace jh
{
	class RoomManager;

	class Room : public JobQueue
	{
	public:
		using UnicastFunc = std::function<void(ULONGLONG, PacketBufferRef)>;
		using OnEmptyFunc = std::function<void(USHORT)>;
		using OnGameStartFunc = std::function<void(USHORT)>;

		enum class RoomState : byte
		{
			IDLE = 0, // 대기 중인 방.
			RUNNING, // 게임 중인 방.
		};

		enum class RoomEnterResult : byte
		{
			SUCCESS = 0,
			FULL,
			ALREADY_STARTED,
			DESTROYED,
		};

		Room(const RoomInfo& roomInfo,const UnicastFunc& unicast,const OnEmptyFunc& onRoomEmpty, const OnGameStartFunc& onGameStart);
		~Room();

		void OnCreated(UserRef owner);

		void TryEnterRoom(UserRef user);
		void LeaveRoom(UserRef user);

		
		void Chat(UserRef sender, PacketBufferRef packet);
		// 유저 레디 상태를 변경. 모두 레디(마지막 방장 레디) -> 게임 실행
		void UpdateUserReadyStatus(UserRef user, bool isReady, bool sendOpt);
		
		RoomInfo GetRoomInfo() const		{ return m_roomInfo; }
		USHORT GetCurUserCnt() const		{ return m_roomInfo.m_usCurUserCnt; }
		USHORT GetMaxUserCnt() const		{ return m_roomInfo.m_usMaxUserCnt; }
		USHORT GetRoomNum() const			{ return m_roomInfo.m_usRoomNum; }
		ULONGLONG GetOwnerId() const		{ return m_roomInfo.m_ullOwnerId; }
		const WCHAR* GetRoomNamePtr() const	{ return m_roomInfo.m_wszRoomName; }
		USHORT GetReadyCnt() const			{ return m_usReadyCnt; }

		bool IsRoomEmpty() const			{ return 0 == GetCurUserCnt(); }
		static LONG GetAliveRoomCount()		{ return aliveRoomCount; }
		
		void BroadCast(PacketBufferRef packet,const ULONGLONG excludedId = 0); // 패킷, 방번호, 제외할 UserID (없으면 0)
	private:
		USHORT ReleaseUser(UserRef user);

		void Unicast(PacketBufferRef packet, ULONGLONG sessionId) { m_unicastFunc(sessionId, packet); }
	
		void OnRoomEmpty() { m_onRoomEmpty(GetRoomNum()); }
		void OnGameStart() { m_onGameStart(GetRoomNum()); }
	private:
		static alignas(32) LONG								aliveRoomCount;

		RoomInfo														m_roomInfo;
		RoomState														m_roomState = RoomState::IDLE;
		USHORT															m_usReadyCnt;

		std::unordered_map<ULONGLONG, std::weak_ptr<jh::User>>	m_userMap; // USER ID - USER  
		
		UnicastFunc														m_unicastFunc;
		OnEmptyFunc														m_onRoomEmpty;
		OnGameStartFunc													m_onGameStart;
	};
}
