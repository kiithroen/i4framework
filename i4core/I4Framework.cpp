#include "stdafx.h"
#include "I4Framework.h"
#include "I4Log.h"
#include "I4FrameCallback.h"
#include "I4FrameStateMgr.h"
#include "I4FrameTimer.h"
#include "I4StopWatch.h"

#ifdef _WIN32
#include "I4FrameworkWin.h"
#endif

namespace i4core {

	I4Framework::I4Framework()
		: windowID(nullptr)
		, title("Untitled")
		, width(800)
		, height(600)
		, frameCallback(nullptr)
	{
		for (int i = 0; i < 256; ++i)
		{
			keyPressed[i] = false;
		}
	}

	I4Framework::~I4Framework()
	{
		destroy();
	}

	bool I4Framework::create(unsigned int _width, unsigned int _height, const string& _title)
	{
		width	= _width;
		height	= _height;
		title	= _title;

		I4FrameTimer::createFrameTimer();
		I4StopWatch::initialize();

		if (onCreate() == false)
		{
			destroy();
			return false;
		}

		return true;
	}

	void I4Framework::destroy()
	{
		I4FrameTimer::destroyFrameTimer();

		onDestroy();
	}

	void I4Framework::run()
	{
		int frameCount = 0;
		float elapsedSec = 0;
		float fps = 0;

		if (frameCallback)
		{
			if (frameCallback->onStart() == false)
			{
				frameCallback->onEnd();
				return;
			}			
		}

		I4FrameTimer::getFrameTimer()->update();

		while (onRun())
		{
			float deltaSec = I4FrameTimer::getFrameTimer()->update();
			elapsedSec += deltaSec;
			frameCount += 1;
			if (elapsedSec >= 1.0f)
			{
				fps = (float)frameCount/elapsedSec;

				elapsedSec = 0;
				frameCount = 0;

				I4LOG_INFO << "fps = " << fps;
			}

			if (mainLoop(deltaSec) == false)
				break;
		}

		if (frameCallback)
		{
			frameCallback->onEnd();
		}
	}

	bool I4Framework::mainLoop(float deltaSec)
	{
		static float elapsedSec = 0;
		const float TIMING = 1.0f/60;

		elapsedSec += deltaSec;
		if (elapsedSec >= TIMING)
		{
			while (elapsedSec >= TIMING)
			{
				elapsedSec -= TIMING;

				if (frameCallback)
				{
					if (frameCallback->onUpdate(TIMING) == false)
						return false;
				}
			}
		}
		else
		{
			onYield();
		}	

		if (frameCallback)
		{
			if (frameCallback->onRender(deltaSec) == false)
				return false;
		}

		return true;
	}

	void I4Framework::setFrameCallback(I4FrameCallback* callback)
	{
		frameCallback = callback;
	}

	//-----------------------------------------------------------------------------

	I4Framework* I4Framework::framework = nullptr;

	void I4Framework::createFramework()
	{
		assert(framework == nullptr);

#ifdef _WIN32
		framework = new I4FrameworkWin;
#else
		I4LOG_WARN << "not implement yet...";
		assert(false);
		framework = new I4Framework;		
#endif	
	}

	void I4Framework::destroyFramework()
	{
		delete framework;
		framework = nullptr;
	}
}