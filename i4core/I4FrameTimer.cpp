#include "stdafx.h"
#include "I4FrameTimer.h"

#ifdef _WIN32
#include "I4FrameTimerWin.h"
#endif

namespace i4core
{

	I4FrameTimer* I4FrameTimer::timer = nullptr;

	I4FrameTimer::I4FrameTimer()
		: dt(1.0f/30.0f)
	{
	}

	I4FrameTimer::~I4FrameTimer()
	{
	}

	void I4FrameTimer::createFrameTimer()
	{
		assert(timer == nullptr);

#ifdef _WIN32
		timer = new I4FrameTimerWin;
#else
		I4LOG_WARN << "not implement yet...;
		assert(false);
		timer = new I4FrameTimer;		
#endif	
	}

	void I4FrameTimer::destroyFrameTimer()
	{
		delete timer;
		timer = nullptr;
	}
}