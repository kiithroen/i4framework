#include "stdafx.h"
#include "I4Framework.h"
#include "I4Log.h"
#include "I4FrameCallback.h"
#include "I4FrameTimer.h"
#include "I4StopWatch.h"
#include "I4Profile.h"
#include "I4ProfileWriterLog.h"

#ifdef _WIN32
#include "I4FrameworkWin.h"
#endif

namespace i4core {

	int	 I4InputState::mouseX;
	int	 I4InputState::mouseY;
	bool I4InputState::KeyPressed[256];
	bool I4InputState::LeftMousePressed;
	bool I4InputState::RightMousePressed;

	I4Framework::I4Framework()
		: windowID(nullptr)
		, title("Untitled")
		, width(800)
		, height(600)
		, fps(0)
		, frameCallback(nullptr)
	{
		for (int i = 0; i < 256; ++i)
		{
			I4InputState::KeyPressed[i] = false;
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
			float dt = I4FrameTimer::getFrameTimer()->update();
			if (mainLoop(dt) == false)
				break;

			elapsedSec += dt;
			frameCount += 1;
			if (elapsedSec >= 1.0f)
			{
				fps = (float)frameCount/elapsedSec;

				elapsedSec = 0;
				frameCount = 0;

				I4ProfileWriterLog	writer;
				writer.write(I4ProfileManager::getRootNode(), fps);

				I4ProfileManager::clear();
			}
		}

		if (frameCallback)
		{
			frameCallback->onEnd();
		}
	}

	bool I4Framework::mainLoop(float dt)
	{
		static float elapsedSec = 0;
		const float TIMING = 1.0f/60;

		elapsedSec += dt;

		if (elapsedSec >= TIMING)
		{
			while (elapsedSec >= TIMING)
			{
				elapsedSec -= TIMING;

				if (frameCallback)
				{
					if (frameCallback->onSimulate(TIMING) == false)
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
			if (frameCallback->onUpdate() == false)
				return false;

			if (frameCallback->onRender() == false)
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