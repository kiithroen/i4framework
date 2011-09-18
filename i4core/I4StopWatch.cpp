#include "I4StopWatch.h"

using namespace i4core;

double I4StopWatch::ticksPerSec = 0;

void I4StopWatch::initialize()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	ticksPerSec = (double)freq.QuadPart;
}