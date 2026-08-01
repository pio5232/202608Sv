#include "LibraryPch.h"
#include "Timer.h"


jh_content::Timer::Timer() : m_callback{}
{
	InitializeSRWLock(&m_lock);
}

jh_content::Timer::~Timer()
{
	SRWLockGuard lockGuard(&m_lock);
	
	m_callback = nullptr;
}
void jh_content::Timer::Start(ULONGLONG interval, CallbackType&& callback)
{
	SRWLockGuard lockGuard(&m_lock);
	
	if (nullptr != m_callback)
		throw custom_exception::timer_already_running_exception();

	m_callback = std::move(callback);

	SetInterval(interval);

	DoTimer(interval, &Timer::OnTimer);
}

void jh_content::Timer::Stop()
{
	SRWLockGuard lockGuard(&m_lock);

	m_callback = nullptr;
}

void jh_content::Timer::OnTimer()
{
	{
		SRWLockGuard lockGuard(&m_lock);

		if (nullptr == m_callback)
			return;

		m_callback();
	}

	DoTimer(m_ullInterval, &Timer::OnTimer);
}

void jh_content::StartTimer(Timer& timer, ULONGLONG interval, CallbackType callback)
{
	timer.Start(interval, std::move(callback));
}
