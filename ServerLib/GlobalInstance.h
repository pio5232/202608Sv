#pragma once
namespace jh_utility
{
	class SerializationBuffer;
	class SessionConnectionEvent;
	class FileLogger;
}

namespace jh
{
	class MemorySystem;
}

namespace jh
{
	class GlobalQueue;
	class Job;
	class JobTimer;
}

extern jh::MemorySystem				* g_pMemSystem;
extern jh_utility::FileLogger				* g_pLogger;

extern jh::GlobalQueue				* g_pGlobalQueue;
extern jh::JobTimer					* g_pJobTimer;
extern thread_local ULONGLONG				g_tlsEndTickCount;
extern thread_local jh::JobQueue	* g_tlsCurrentJobQueue;
