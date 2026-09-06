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
		virtual ~SingleIocpServer() override = default;

		// 기존 IocpServer에서 상속받은 함수들은 재정의하지않고 자식 클래스로 넘김

	private:
		virtual void OnWorkerThreadUpdateBegin() override;
		virtual void OnWorkerThreadUpdateEnd() override;
		virtual void OnInitialized() override;

		virtual ServerConfig* CreateConfig() override;

	};
}
