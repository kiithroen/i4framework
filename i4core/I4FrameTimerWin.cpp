#include "stdafx.h"
#include "I4FrameTimerWin.h"

namespace i4core
{
	I4FrameTimerWin::I4FrameTimerWin()
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&last);

		dt = update();
	}

	I4FrameTimerWin::~I4FrameTimerWin()
	{
	}
	
	float I4FrameTimerWin::update()
	{
		QueryPerformanceCounter(&current);
		dt = (float)((double)(current.QuadPart - last.QuadPart)/(double)frequency.QuadPart); // �ð��� ��� (����ð� - �����ð�)
		last = current;

		return dt;
	}

}