#include "stdafx.h"
#include "I4FrameTimer.h"

namespace i4core
{

	I4FrameTimer* I4FrameTimer::timer = nullptr;

	I4FrameTimer::I4FrameTimer(float _tickInterval)
		: frameDelta(0)
		, tickInterval(_tickInterval)
		, elapsedAfterLastTick(0)
		, tickCount(0)
	{
		
	}

	I4FrameTimer::~I4FrameTimer()
	{
	}

	float I4FrameTimer::updateFrameDelta()
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
		I4LOG_WARN << "not implement yet...";
		assert(false);
		return 0;
#endif
	}

	void I4FrameTimer::createFrameTimer(float tickInterval)
	{
		assert(timer == nullptr);

		timer = new I4FrameTimer(tickInterval);
	}

	void I4FrameTimer::destroyFrameTimer()
	{
		delete timer;
		timer = nullptr;
	}
}