#pragma once

#include <MSWSock.h>

namespace jh
{
	/*--------------------
		  NetAddress
	--------------------*/
	class NetAddress
	{
	public:
		static void Init();
		static void Clear();

		NetAddress(SOCKADDR_IN sockAddr);
		NetAddress(std::wstring ip, USHORT port);
		NetAddress(const NetAddress& other);
		NetAddress() {}

		NetAddress& operator=(const NetAddress& other);
		NetAddress& operator=(const SOCKADDR_IN& sockAddr);

		void Reset() { m_sockAddr = {}; }
		void Init(SOCKADDR_IN sockAddr);
		const SOCKADDR_IN& GetSockAddr() const { return m_sockAddr; }
		const std::wstring	GetIpAddress() const;
		const USHORT GetPort() const { return ntohs(m_sockAddr.sin_port); }

		static IN_ADDR IpToAddr(const WCHAR* ip);
		static USHORT GetPort(SOCKET sock);

		static LPFN_CONNECTEX	lpfnConnectEx;
	private:
		SOCKADDR_IN				m_sockAddr = {};
	};
	
}

struct ServerConfig
{
	virtual ~ServerConfig() {}
	WCHAR		m_wszIp[IP_STRING_LEN]{};									// ip
	USHORT		m_usPort;												// 포트 번호
	DWORD		m_dwMaxSessionCnt;										// 한번에 접속가능한 최대 세션 수
	DWORD		m_dwConcurrentWorkerThreadCount;						// iocp에 등록할 worker 수
	LINGER		m_lingerOption;											// TIME_OUT 옵션 설정
	ULONGLONG	m_ullTimeoutLimit;										// HEARTBEAT
	ULONGLONG	m_ullTimeoutCheckInterval;								// HEARTBEAT 체크 주기
	// ULONGLONG	m_ullWorkerTick;
};


struct MultiServerConfig : public ServerConfig
{
	virtual ~MultiServerConfig() override {}

	ULONGLONG m_ullWorkerTick;
};

struct LobbyServerConfig : MultiServerConfig
{
	virtual ~LobbyServerConfig() override {}

	USHORT m_usMaxRoomCnt;
	USHORT m_usMaxRoomUserCnt;
};