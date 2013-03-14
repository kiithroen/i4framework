#include "stdafx.h"
#include "I4StopWatch.h"

namespace i4core
{
	LARGE_INTEGER I4StopWatch::frequency;

	I4StopWatch::I4StopWatch()
	{
		static bool initialize = false;
		if (initialize == false)
		{
			QueryFrequency();
			QueryCounter(&last);
			initialize = true;
		}
	}

	void I4StopWatch::reset()
	{
#ifdef _WIN32
		QueryCounter(&last);
#else
		I4LOG_WARN << "not implement yet...";
		assert(false);
#endif
	}

	float I4StopWatch::elapsed()
	{
#ifdef _WIN32
		QueryCounter(&current);
		return (float)((double)(current.QuadPart - last.QuadPart)/(double)frequency.QuadPart);
#else
		I4LOG_WARN << "not implement yet...";
		assert(false);
		return 0;
#endif
	}

	void I4StopWatch::QueryCounter(LARGE_INTEGER* count)
	{
		// ��Ƽ�ھ�� �ھ�� �ٸ� �ð����� �������� ������ �־ �����ų �ھ ������Ŵ.
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
		QueryPerformanceCounter(count);
		SetThreadAffinityMask(::GetCurrentThread(), oldmask);
	}

	void I4StopWatch::QueryFrequency()
	{
		QueryPerformanceFrequency(&frequency);
	}

}