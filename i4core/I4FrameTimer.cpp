#include "stdafx.h"
#include "I4FrameTimer.h"

namespace i4core
{

	FrameTimer* FrameTimer::timer = nullptr;

	FrameTimer::FrameTimer(float _tickInterval)
		: frameDelta(0)
		, tickInterval(_tickInterval)
		, elapsedAfterLastTick(0)
		, tickCount(0)
	{
		
	}

	FrameTimer::~FrameTimer()
	{
	}

	float FrameTimer::updateFrameDelta()
	{
#ifdef _WIN32
		// 멀티코어에서 코어마다 다른 시간값을 가져오는 문제가 있어서 실행시킬 코어를 고정시킴.
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);

		static bool initialize = false;
		if (initialize == false)
		{
			QueryPerformanceFrequency(&frequency);
			QueryPerformanceCounter(&last);

			initialize = true;
		}
		
		QueryPerformanceCounter(&current);
		
		SetThreadAffinityMask(::GetCurrentThread(), oldmask);

		frameDelta = (double)(current.QuadPart - last.QuadPart)/(double)frequency.QuadPart; // 시간차 계산 (현재시간 - 이전시간)
		last = current;

		elapsedAfterLastTick += frameDelta;

		return (float)frameDelta;
#else
		LOG_WARN << "not implement yet...";
		assert(false);
		return 0;
#endif
	}

	void FrameTimer::createFrameTimer(float tickInterval)
	{
		assert(timer == nullptr);

		timer = new FrameTimer(tickInterval);
	}

	void FrameTimer::destroyFrameTimer()
	{
		delete timer;
		timer = nullptr;
	}
}