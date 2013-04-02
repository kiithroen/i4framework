#pragma once

namespace i4core
{
	class StopWatch
	{
	public:
		StopWatch();

		void QueryFrequency();

		void reset();

		void QueryCounter(LARGE_INTEGER* count);

		float elapsed();
		
#ifdef _WIN32
	private:
		static LARGE_INTEGER	frequency;

	private:
		LARGE_INTEGER	last;
		LARGE_INTEGER	current;
#endif
	};
}
