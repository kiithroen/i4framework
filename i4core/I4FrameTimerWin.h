#pragma once

#include "I4FrameTimer.h"

namespace i4core
{
	class I4FrameTimerWin : public I4FrameTimer
	{
		friend class I4FrameTimer;
	public:
		virtual ~I4FrameTimerWin();

		virtual float	update();

	private:
		I4FrameTimerWin();

	private:
		LARGE_INTEGER	frequency;
		LARGE_INTEGER	last;
		LARGE_INTEGER	current;
	};
}