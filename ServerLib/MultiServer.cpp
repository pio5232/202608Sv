#include "LibraryPch.h"
#include "MultiServer.h"

/*-----------------------
	  MultiServer
-----------------------*/

jh::MultiIocpServer::MultiIocpServer(const WCHAR* serverName) : IocpServer{ serverName }
{
	m_pJobQueue = jh::MakeShared<JobQueue>();
}



void jh::IocpServer::InitializeServerTasks()
{
	MultiServerConfig* multiServerConfig = static_cast<MultiServerConfig*>(&m_config);

	m_workerExecutor.Run([this]()
		{
			ProcessAccept();
		});


	int creationCount = multiServerConfig->m_dwConcurrentWorkerThreadCount * 1.5;

	
	for (int i = 0; i < creationCount; i++)
	{
		m_workerExecutor.Run([this]()
			{
				bool isRunning = true;

				while (isRunning)
				{
					g_tlsEndTickCount = jh_utility::GetTimeStamp() + m_config.m_ullWorkerTick;

					isRunning = ProcessIO(10);

					OnWorkerThreadUpdate();
				}
			});
	}
	m_pJobQueue->DoTimer(m_config.m_ullTimeoutCheckInterval, [this]() {this->OnHeartbeatTimer(); });
	

	return;
}


void jh::MultiIocpServer::OnHeartbeatTimer()
{
	const ULONGLONG checkInterval = GetConfig()->m_ullTimeoutCheckInterval;

	CheckHeartbeatTimeout();

	m_pJobQueue->DoTimer(checkInterval, [this]() {this->OnHeartbeatTimer(); });
}



MultiServerConfig* jh::MultiIocpServer::CreateConfig()
{
	return new MultiServerConfig{};
}
