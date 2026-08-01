#include "LibraryPch.h"
#include "SingleServer.h"

/*-----------------------
	  SingleServer
-----------------------*/

jh::SingleIocpServer::SingleIocpServer(const WCHAR* serverName) : IocpServer{ serverName }
{

}

void jh::SingleIocpServer::InitializeServerTasks()
{
	m_workerExecutor.Run([this]()
		{
			ProcessAccept();
		});

	int creationCount = m_config.m_dwConcurrentWorkerThreadCount * 1.5;

	m_workerExecutor.Run([this]()
		{

		});
	for (int i = 0; i < creationCount; i++)
	{
		m_workerExecutor.Run([this]()
			{
				bool isRunning = true;

				while (isRunning)
				{
					isRunning = ProcessIO(10);
				}
			});

	}

	return;
}


ServerConfig* jh::SingleIocpServer::CreateConfig()
{
	return new ServerConfig{}; 
}