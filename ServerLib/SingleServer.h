#pragma once
#include "Session.h"
//#include "PacketDefine.h"

namespace jh
{
	/*-----------------------------
			IocpSingleServer
	------------------------------*/

	// Iocp 스레드는 네트워크 관련 작업만. 로직 스레드를 Single로 두어 사용한다.

	class SingleIocpServer : public IocpServer
	{
	public:
		SingleIocpServer(const WCHAR* serverName);
		~SingleIocpServer() {}

		// 각각의 함수들은 Start() / Stop()가 실행됐을 때
		// 상속받은 함수에서 추가적으로 작업할 것들을 여기에 등록하면 된다.
		virtual void OnStart() override = 0;
		virtual void OnStop() override = 0;

		virtual bool OnConnectionRequest(const SOCKADDR_IN& clientInfo) override = 0;
		virtual void OnError(int errCode, WCHAR* cause) override = 0;

		virtual void OnRecv(ULONGLONG sessionId, PacketBufferRef dataBuffer, USHORT type) override = 0;

		virtual void OnConnected(ULONGLONG sessionId) override = 0;
		virtual void OnDisconnected(ULONGLONG sessionId) override = 0;

		//virtual void OnWorkerThreadUpdate() = 0;

		virtual void InitializeServerTasks() override;
		virtual ServerConfig* CreateConfig() override;

	};
}
