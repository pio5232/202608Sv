#pragma once

namespace jh
{
	class DummyUpdateSystem
	{
	public:
		using PacketFunc = void(DummyUpdateSystem::*)(ULONGLONG, PacketBufferRef&, int);

		struct PendingData
		{
			// RoomListRequestPacket
			// Chat
			// MakeRoom
			// EnterRoom
			// LeaveRoom
			// Login

			LONG m_lRoomListPacket = 0;
			LONG m_lChatPacket = 0;
			LONG m_lMakeRoomPacket = 0;
			LONG m_lEnterRoomPacket = 0;
			LONG m_lLeaveRoomPacket = 0;
			LONG m_lLoginPacket = 0;

			PendingData& operator+=(const PendingData& other)
			{
				m_lRoomListPacket += other.m_lRoomListPacket;
				m_lChatPacket += other.m_lChatPacket;
				m_lMakeRoomPacket += other.m_lMakeRoomPacket;
				m_lEnterRoomPacket += other.m_lEnterRoomPacket;
				m_lLeaveRoomPacket += other.m_lLeaveRoomPacket;
				m_lLoginPacket += other.m_lLoginPacket;

				return *this;
			}
		};
		struct EtcData
		{
			LONG m_lDestroyedRoomErrorCount = 0;
			LONG m_lDiffRoomNameErrorCount = 0;
			LONG m_lFullRoomErrorCount = 0;
			LONG m_lAlreadyRunningRoomErrorCount = 0;
			LONG m_lReSendTimeoutCount = 0;
			LONG m_lDummyDisconnectCount = 0; // 일부러 더미에서 재연결 재현 횟수

			PendingData m_pendingDataForPrint{};
		};
	private:

		struct alignas(64) LogicData
		{

			struct ErrorAggregator
			{
				alignas(32) LONG m_lDestroyedRoom = 0;
				alignas(32) LONG m_lDiffRoomName = 0;
				alignas(32) LONG m_lFullRoom = 0;
				alignas(32) LONG m_lAlreadyRunningRoom = 0;
				alignas(32) LONG m_lDummyDisconnectCount = 0;
			};
		public:
			LogicData();

			ErrorAggregator m_errorAggregator;
			alignas(32) LONG m_reSendTimeoutCnt;

			HANDLE m_hLogicThread;
			HANDLE m_hJobEvent;
			jh_utility::LockQueue<DummyThreadJobRef> m_netJobQueue;
			jh_utility::LockQueue<SessionConnectionEventRef> m_sessionConnEventQueue;
			std::unordered_map<ULONGLONG, DummyRef> m_dummyUmap;
			std::unordered_map<DummyRef, int> m_dummyVectorIndexUMap;
			std::vector<DummyRef> m_dummyVector;
			PendingData m_pendingData;

		};

		struct WorkerTransData
		{
			void* m_pThis;
			int m_iThreadNum;
		};

	private:
		void ProcessPacket(ULONGLONG sessionId, DWORD packetType, PacketBufferRef& packet, int threadNum);

	public:
		static unsigned WINAPI LogicThreadFunc(LPVOID lparam);
		void DummyLogic(int threadNum);

		DummyUpdateSystem(jh::IocpClient* owner);
		~DummyUpdateSystem();

		void Init();
		void Stop();

		void EnqueueLogicEvent(DummyThreadJobRef& job, int idx);
		void EnqueueSessionConnEvent(SessionConnectionEventRef& connectionEvent, int idx);
		void ProcessNetJob(int threadNum);
		void ProcessSessionConnectionEvent(int threadNum);

		void ProcessDummyLogic(int threadNum);

		void SendToDummy(DummyRef& dummy, PacketBufferRef& packet, DummyStatus nextStauts = DummyStatus::NO_CHANGE);

		void HandleRoomListResponsePacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		void HandleLogInResponsePacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		void HandleMakeRoomResponsePacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		void HandleEnterRoomResponsePacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		void HandleChatNotifyPacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		void HandleChatResponsePacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		void HandleLeaveRoomResponsePacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		void HandleEchoPacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);

		void HandleErrorPacket(ULONGLONG sessionId, PacketBufferRef& packet, int threadNum);
		bool IsValidThreadNum(int threadNum) { return threadNum >= 0 && threadNum < LOGIC_THREAD_COUNT; }


		EtcData UpdateEtc() const;

		void CheckSendTimeOut(int threadNum);

		ULONGLONG GetRTT() const;

		std::unordered_map<USHORT, PacketFunc>	m_packetFuncDic;
		jh::IocpClient* m_pOwner;

		LogicData								m_logicData[LOGIC_THREAD_COUNT];

		char									m_bRunnigFlag;
		alignas(64) ULONGLONG					m_ullRtt;

	public:
	};
}

