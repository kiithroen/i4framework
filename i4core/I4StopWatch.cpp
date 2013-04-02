#include "stdafx.h"
#include "I4StopWatch.h"

namespace i4core
{
	LARGE_INTEGER StopWatch::frequency;

	StopWatch::StopWatch()
	{
		static bool initialize = false;
		if (initialize == false)
		{
			QueryFrequency();
			QueryCounter(&last);
			initialize = true;
		}
	}

	void StopWatch::reset()
	{
#ifdef _WIN32
		QueryCounter(&last);
#else
		LOG_WARN << "not implement yet...";
		assert(false);
#endif
	}

	float StopWatch::elapsed()
	{
#ifdef _WIN32
		QueryCounter(&current);
		return (float)((double)(current.QuadPart - last.QuadPart)/(double)frequency.QuadPart);
#else
		LOG_WARN << "not implement yet...";
		assert(false);
		return 0;
#endif
	}

	void StopWatch::QueryCounter(LARGE_INTEGER* count)
	{
		// 멀티코어에서 코어마다 다른 시간값을 가져오는 문제가 있어서 실행시킬 코어를 고정시킴.
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
		QueryPerformanceCounter(count);
		SetThreadAffinityMask(::GetCurrentThread(), oldmask);
	}

	void StopWatch::QueryFrequency()
	{
		QueryPerformanceFrequency(&frequency);
	}

}