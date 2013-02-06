#include "I4StopWatch.h"

namespace i4core
{

	double I4StopWatch::ticksPerSec = 0;

	I4StopWatch::I4StopWatch()
	{
		static bool init = false;
		if (init == false)
		{
			initialize();
			init = true;
		}
	}

	void I4StopWatch::reset()
	{
#ifdef _WIN32
		QueryPerformanceCounter(&startTime);
#else
		I4LOG_WARN << "not implement yet...";
		assert(false);
#endif
	}

	float I4StopWatch::getElapsedTime()
	{
#ifdef _WIN32
		QueryPerformanceCounter(&endTime);
		return (float)((double)(endTime.QuadPart - startTime.QuadPart)/ticksPerSec);
#else
		I4LOG_WARN << "not implement yet...";
		assert(false);
		return 0;
#endif
	}

	void I4StopWatch::initialize()
	{
#ifdef _WIN32
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		ticksPerSec = (double)freq.QuadPart;
#else
		I4LOG_WARN << "not implement yet...";
		assert(false);
#endif
	}

}