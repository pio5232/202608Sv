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
	ServerConfig* serverConfig = GetConfig();

	int creationCount = serverConfig->m_dwConcurrentWorkerThreadCount * 1.5;
;
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