#pragma once

namespace jh
{
	class Timer : private JobQueue
	{
	public: 
		Timer();
		~Timer();
		void Start(ULONGLONG interval, CallbackType&& callback);
		void Stop();

	private:
		void OnTimer();
		void SetInterval(ULONGLONG interval) { m_ullInterval = interval > 0 ? interval : 1; }


	private:
		SRWLOCK m_lock;
		ULONGLONG m_ullInterval;
		CallbackType m_callback;
	};

	void StartTimer(Timer& timer,ULONGLONG interval, CallbackType callback);
}

