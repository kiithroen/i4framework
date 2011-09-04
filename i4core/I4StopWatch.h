#pragma once

#include "i4core.h"

namespace i4core
{
	class I4CORE_API I4StopWatch
	{
	public:
		void		reset()
		{
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			startTime = time.QuadPart;
		}

		float		getElapsedTime()
		{
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);

			return (float)((double)(time.QuadPart - startTime)/ticksPerSec);
		}
		
	private:
		LONGLONG		startTime;

	public:		
		static void initialize()
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			ticksPerSec = (double)freq.QuadPart;
		}

	private:
		static double		ticksPerSec;
	};
}
