#include "LibraryPch.h"
#include "MultiServer.h"

/*-----------------------
	  MultiServer
-----------------------*/

jh::MultiIocpServer::MultiIocpServer(const WCHAR* serverName) : IocpServer{ serverName }
{
	m_pJobQueue = jh::MakeShared<JobQueue>();
}

void MultiIocpServer::OnStarted()
{
}

void MultiIocpServer::OnStop()
{
}


void jh::MultiIocpServer::OnHeartbeatTimer()
{
	const ULONGLONG checkInterval = GetConfig()->m_ullTimeoutCheckInterval;

	CheckHeartbeatTimeout();

	m_pJobQueue->DoTimer(checkInterval, [this]() {this->OnHeartbeatTimer(); });
}

void MultiIocpServer::OnWorkerThreadUpdateBegin()
{
	const MultiServerConfig* multiCfgPtr = static_cast<const MultiServerConfig*>(GetConfig());
	g_tlsEndTickCount = jh_utility::GetTimeStamp() + multiCfgPtr->m_ullWorkerTick;
}

void MultiIocpServer::OnWorkerThreadUpdateEnd()
{
	
}

void MultiIocpServer::OnInitialized()
{
	const MultiServerConfig* multiCfgPtr = static_cast<const MultiServerConfig*>(GetConfig());

	m_pJobQueue->DoTimer(multiCfgPtr->m_ullTimeoutCheckInterval, [this]() {this->OnHeartbeatTimer(); });
}

MultiServerConfig* jh::MultiIocpServer::CreateConfig()
{
	return static_cast<MultiServerConfig*>(g_pMemSystem->Alloc((sizeof(MultiServerConfig))));
}
