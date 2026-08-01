#pragma once
#include "Session.h"
//#include "PacketDefine.h"

namespace jh
{
	/*-------------------------
			IocpMultiServer
	-------------------------*/

	// Iocp 스레드가 네트워크 + 로직까지 처리하도록 한다.

	class MultiIocpServer : public IocpServer
	{
	public:
		MultiIocpServer(const WCHAR* serverName);
		~MultiIocpServer() {}

		// 각각의 함수들은 Start() / Stop()가 실행됐을 때
		// 상속받은 함수에서 추가적으로 작업할 것들을 여기에 등록하면 된다.
		virtual void OnStart() override = 0;
		virtual void OnStop() override = 0;

		virtual bool OnConnectionRequest(const SOCKADDR_IN& clientInfo) override = 0;
		virtual void OnError(int errCode, WCHAR* cause) override = 0;

		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef dataBuffer, USHORT type) override = 0;

		virtual void OnConnected(ULONGLONG sessionId) override = 0;
		virtual void OnDisconnected(ULONGLONG sessionId) override = 0;

		virtual void InitializeServerTasks() override;

		// 파생 클래스에서 해야할 작업을 정의하도록 한다.
		virtual void OnWorkerThreadUpdate() = 0;

	private:
		void OnHeartbeatTimer();

		virtual MultiServerConfig* CreateConfig() override;

		JobQueueRef						m_pJobQueue;							// 하트비트 체크 타이머를 위한 잡큐

	};
}