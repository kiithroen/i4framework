#pragma once

#include "i4core.h"

namespace i4core
{
	class I4CORE_API I4StopWatch
	{
	public:
		void reset()
		{
			QueryPerformanceCounter(&startTime);
		}

		float getElapsedTime()
		{
			QueryPerformanceCounter(&endTime);

			return (float)((double)(endTime.QuadPart - startTime.QuadPart)/ticksPerSec);
		}
		
	private:
		LARGE_INTEGER		startTime;
		LARGE_INTEGER		endTime;

	public:		
		static void initialize();

	private:
		static double	ticksPerSec;
	};
}
