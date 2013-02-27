#pragma once

namespace i4core
{
	class I4CORE_API I4StopWatch
	{
	public:
		I4StopWatch();

		void reset();
		float getElapsedTime();
		
	private:
#ifdef _WIN32
		LARGE_INTEGER		startTime;
		LARGE_INTEGER		endTime;
#endif
	public:		
		static void initialize();

	private:
		static double	ticksPerSec;
	};
}
