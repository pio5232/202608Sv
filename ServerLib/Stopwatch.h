#pragma once

namespace jh_utility
{
	/*----------------------
			Stopwatch
	----------------------*/
	class Stopwatch
	{
	public:
		Stopwatch();
		void Start() { QueryPerformanceCounter(&m_start); }

		// end - start 
		double Stop()
		{
			QueryPerformanceCounter(&m_end);

			double ret = static_cast<double>(m_end.QuadPart - m_start.QuadPart) / frequency.QuadPart;

			m_start.QuadPart = 0;

			return ret;
		}

		// end - start
		// start = end
		double Lap()
		{
			QueryPerformanceCounter(&m_end);

			double ret = static_cast<double>(m_end.QuadPart - m_start.QuadPart) / frequency.QuadPart;

			m_start = m_end;

			return ret;
		}
		static ULONGLONG GetFrequency() { return frequency.QuadPart; }
	private:
		LARGE_INTEGER m_start;
		LARGE_INTEGER m_end;
		const inline static LARGE_INTEGER frequency = []() 
			{
				LARGE_INTEGER fq; 
				QueryPerformanceFrequency(&fq); 
				return fq; 
			}();

	};

	// ms는 *1000
	// micro는 *1000 *1000

	// 서버와 클라이언트에서 동일한 로직을 사용하도록 맞춰줘야함.

	inline ULONGLONG GetTimeStamp()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	inline ULONGLONG GetTimeStampMicrosecond()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

}

