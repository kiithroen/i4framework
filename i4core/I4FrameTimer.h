#pragma once

#include "i4core.h"

namespace i4core
{
	class I4CORE_API I4FrameTimer
	{
	public:
		virtual ~I4FrameTimer();

		virtual float	update()		{ return deltaSec; }

		float			getDeltaSec()	{ return deltaSec; }

	protected:
		I4FrameTimer();

	public:
		static I4FrameTimer*	getFrameTimer()	{ return timer; }

		static void		createFrameTimer();
		static void		destroyFrameTimer();

	private:
		static I4FrameTimer*	timer;

	protected:
		float			deltaSec;
	};
}