#include "LibraryPch.h"
#include "ThreadExecutor.h"
#include "GlobalQueue.h"

jh_utility::ThreadExecutor::ThreadExecutor()
{
	InitializeSRWLock(&m_lock);
}

jh_utility::ThreadExecutor::~ThreadExecutor()
{
	Join();
}

void jh_utility::ThreadExecutor::Run(std::function<void()> callback)
{
	SRWLockGuard lockGuard(&m_lock);

	m_threads.push_back(
		std::thread([=]() {callback(); })
	);
}

void jh_utility::ThreadExecutor::Join()
{
	SRWLockGuard lockGuard(&m_lock);

	for (std::thread& t : m_threads)
	{
		if (t.joinable())
			t.join();
	}

	m_threads.clear();
}

void jh_utility::ThreadExecutor::DoGlobalQueueWork()
{
	while (true)
	{
		ULONGLONG now = jh_utility::GetTimeStamp();
		if (now > g_tlsEndTickCount)
			break;

		JobQueueRef jobQueue = g_pGlobalQueue->Pop();
		if (nullptr == jobQueue)
			break;

		jobQueue->Execute();
	}
}

void jh_utility::ThreadExecutor::DistributeReservedJobs()
{
	const ULONGLONG now = jh_utility::GetTimeStamp();

	g_pJobTimer->Distribute(now);
}
