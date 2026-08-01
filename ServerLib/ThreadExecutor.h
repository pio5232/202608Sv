#pragma once
#include <Windows.h>
#include <functional>
namespace jh_utility
{
	/*--------------------------
			ThreadExecutor
	--------------------------*/

	class ThreadExecutor
	{
	public:
		ThreadExecutor();
		~ThreadExecutor();

		void Run(std::function<void()> callback);
		void Join();

		static void DoGlobalQueueWork();
		static void DistributeReservedJobs();

	private:
		SRWLOCK					 m_lock;
		std::vector<std::thread> m_threads;
	};
}