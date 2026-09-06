#include "LibraryPch.h"
#include "SingleServer.h"

/*-----------------------
	  SingleServer
-----------------------*/

jh::SingleIocpServer::SingleIocpServer(const WCHAR* serverName) : IocpServer{ serverName }
{

}

void SingleIocpServer::OnWorkerThreadUpdateBegin()
{
}

void SingleIocpServer::OnWorkerThreadUpdateEnd()
{
}

void SingleIocpServer::OnInitialized()
{
}

ServerConfig* jh::SingleIocpServer::CreateConfig()
{
	return static_cast<ServerConfig*>(g_pMemSystem->Alloc(sizeof(ServerConfig)));
}
