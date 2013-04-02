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
		// ��Ƽ�ھ�� �ھ�� �ٸ� �ð����� �������� ������ �־ �����ų �ھ ������Ŵ.
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

		frameDelta = (double)(current.QuadPart - last.QuadPart)/(double)frequency.QuadPart; // �ð��� ��� (����ð� - �����ð�)
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