#pragma once

#include "i4core.h"

namespace i4core
{
	class FrameTimer
	{
	public:
		virtual ~FrameTimer();

		float			updateFrameDelta();		

		void			updateTickCount()				{ ++tickCount; elapsedAfterLastTick = 0;}
		void			resetTickCount()				{ tickCount = 0; elapsedAfterLastTick = 0; }

		float			getFrameDelta() const			{ return (float)frameDelta; }
		unsigned int	getTickCount() const			{ return tickCount; }
		float			getTickInterval() const			{ return (float)tickInterval; }

		float			getElapsed() const				{ return (float)((double)tickCount*tickInterval + elapsedAfterLastTick); }
	protected:
		FrameTimer(float tickInterval);

	public:
		static FrameTimer*	getFrameTimer()	{ return timer; }

		static void		createFrameTimer(float tickInterval);
		static void		destroyFrameTimer();

	private:
		static FrameTimer*	timer;

	protected:
		double			frameDelta;
		double			elapsedAfterLastTick;
		double			tickInterval;
		unsigned int	tickCount;

#ifdef _WIN32
	private:
		LARGE_INTEGER	frequency;
		LARGE_INTEGER	last;
		LARGE_INTEGER	current;
#endif
	};
}