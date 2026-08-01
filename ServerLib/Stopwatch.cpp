#include "LibraryPch.h"
#include "Stopwatch.h"

jh_utility::Stopwatch::Stopwatch() : m_start{}, m_end{} {};

// 서버와 클라이언트에서 동일한 로직을 사용하도록 맞춰줘야함.
//ULONGLONG jh_utility::GetTimeStamp()
//{
//	LARGE_INTEGER t;
//	QueryPerformanceCounter(&t);
//
//	return (t.QuadPart * 1'000) / jh_utility::Stopwatch::GetFrequency();
//}
//
//ULONGLONG jh_utility::GetTimeStampMicrosecond()
//{
//	LARGE_INTEGER t;
//	QueryPerformanceCounter(&t);
//
//	return (t.QuadPart * 1'000'000) / jh_utility::Stopwatch::GetFrequency();
//}