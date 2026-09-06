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
		virtual ~MultiIocpServer() override = default;

		// 기존 IocpServer에서 상속받은 함수들은 재정의하지않고 자식 클래스로 넘김

	private:
		void OnStarted() override;
		void OnStop() override;

		void OnHeartbeatTimer();

		virtual void OnWorkerThreadUpdateBegin() override;
		virtual void OnWorkerThreadUpdateEnd() override;
		virtual void OnInitialized() override;
		
		virtual MultiServerConfig* CreateConfig() override;

		JobQueueRef						m_pJobQueue;							// 하트비트 체크 타이머를 위한 잡큐

	};
}