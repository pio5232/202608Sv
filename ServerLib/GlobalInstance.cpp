#include "LibraryPch.h"
#include "GlobalInstance.h"
#include "GlobalQueue.h"
#include "JobTimer.h"

jh_utility::FileLogger* g_pLogger = nullptr;
jh::MemorySystem* g_pMemSystem = nullptr;
jh::GlobalQueue* g_pGlobalQueue = nullptr;
jh::JobTimer* g_pJobTimer = nullptr;

extern thread_local ULONGLONG g_tlsEndTickCount = 0;
extern thread_local jh::JobQueue* g_tlsCurrentJobQueue = nullptr;

jh_utility::CrashDump dump;

class GlobalGenerator
{
public:
	GlobalGenerator()
	{
		PRO_RESET;

		g_pMemSystem = new jh::MemorySystem();
		g_pLogger = new jh_utility::FileLogger();
		g_pGlobalQueue = new jh::GlobalQueue();
		g_pJobTimer = new jh::JobTimer();
		jh::NetAddress::Init();
	}

	~GlobalGenerator()
	{
		PRO_SAVE("ProfileData.TXT");
	
		delete g_pJobTimer;
		delete g_pGlobalQueue;
		delete g_pLogger;
		delete g_pMemSystem;

		jh::NetAddress::Clear();
	}
} g_globalGen;