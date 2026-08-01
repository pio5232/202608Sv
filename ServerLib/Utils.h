#pragma once
#include <stack>
#include <queue>
#include <math.h>
#include <algorithm>
#include <deque>
#include "UserException.h"

#include "ThreadExecutor.h"
#include "RingBuffer.h"
#include "SerializationBuffer.h"
#include "Vector3.h"
#include "SRWLockGuard.h"
#include "CrashDump.h"


namespace jh_utility
{
	struct ProcessTimeInfo
	{
		FILETIME creationTime;
		FILETIME exitTime;
		// GetProcessTime 실행 시 100 나노 초 기준으로 입력됨.
		FILETIME kernnelTime;
		FILETIME userTime;

		// convert용
		SYSTEMTIME systemTime;

		ULONGLONG prevKernelTime;
		ULONGLONG prevUserTime;
	};
	// 기본 자료형 사용
	template <typename T>
	class LockStack
	{
	public:
		LockStack()
		{
			InitializeSRWLock(&m_lock);
		}
		void Reserve(int reserveCount)
		{
			SRWLockGuard lockGuard(&m_lock);
			m_vec.reserve(reserveCount);
		}
		void Push(T data)
		{
			SRWLockGuard lockGuard(&m_lock);
			m_vec.push_back(data);
		}

		bool TryPop(T& data)
		{
			SRWLockGuard lockGuard(&m_lock);
			if (m_vec.size() > 0)
			{
				data = m_vec.back();

				m_vec.pop_back();
				return true;
			}
			else
				return false;
		}
	private:
		SRWLOCK m_lock;
		std::vector<T> m_vec;
	};

	template <typename T>
	class LockQueue {
	public:
		LockQueue()
		{
			InitializeSRWLock(&m_lock);
		}
		void Push(T data)
		{
			SRWLockGuard lockGuard(&m_lock);

			m_queue.push(data);
		}

		T Pop()
		{
			SRWLockGuard lockGuard(&m_lock);
			if (m_queue.size() > 0)
			{
				T ret = m_queue.front();
				m_queue.pop();
				
				return ret;
			}
			
			return T();
		}

		bool TryPop(T& t)
		{
			SRWLockGuard lockGuard(&m_lock);
			if (m_queue.size() > 0)
			{
				t = m_queue.front();
				m_queue.pop();

				return true;
			}
			else
				return false;
		}

		int PopAll(OUT std::vector<T>& vec)
		{
			SRWLockGuard lockGuard(&m_lock);

			int popCount = 0;
			while (m_queue.size() > 0)
			{
				T ele = m_queue.front();
				m_queue.pop();

				vec.push_back(ele);

				popCount++;
			}

			return popCount;
		}

		void Swap(std::queue<T>& q)
		{
			SRWLockGuard lockGuard(&m_lock);
			
			std::swap(m_queue, q);
		}
		int GetUseSize()
		{
			SRWLockGuard lockGuard(&m_lock);
			
			return m_queue.size();
		}
		void Clear()
		{
			SRWLockGuard lockGuard(&m_lock);
			
			m_queue = std::queue<T>();	
		}
	private:
		SRWLOCK m_lock;
		std::queue<T> m_queue;
	};
}

void ExecuteProcess(const WCHAR* path, const WCHAR* currentDirectory);// , const std::wstring& args);



double GetRandDouble(double min, double max, int roundPlaceValue = 0);
int GetRand(int min, int max);

bool CheckChance(int percentage);
