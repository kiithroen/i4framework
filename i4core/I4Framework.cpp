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

	int	 I4InputState::MouseX;
	int	 I4InputState::MouseY;
	bool I4InputState::KeyPressed[256];
	bool I4InputState::LeftMousePressed;
	bool I4InputState::RightMousePressed;
	bool I4InputState::MoveMouseCenter;

	void I4InputState::initialize()
	{
		MouseX = 0;
		MouseY = 0;

		for (int i = 0; i < 256; ++i)
		{
			KeyPressed[i] = false;
		}

		LeftMousePressed = false;
		RightMousePressed = false;
		MoveMouseCenter = false;
	}


	I4Framework::I4Framework()
		: windowID(nullptr)
		, title("Untitled")
		, width(800)
		, height(600)
		, fps(0)
		, frameCallback(nullptr)
	{
		I4InputState::initialize();
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

		// ����� ����ΰ��� ������ ����ΰ�� ����Ÿ�̹�ȣ�� �󵵸� �ٸ��� ��������.
		// ������� ��� �ʹ� ������ ���� �󵵷� ������ �׽�Ʈ�� �ǿ����� �ִ�.
#ifdef _DEBUG
		const float TICK_INTERVAL = 1.0f/30.0f;
#else
		const float TICK_INTERVAL = 1.0f/60.0f;
#endif

		I4FrameTimer::createFrameTimer(TICK_INTERVAL);
		
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

		I4FrameTimer::getFrameTimer()->updateFrameDelta();

		bool done = false;
		while (!done)
		{
			if (onMessagePump() == false)
			{
				done = true;
			}

			float dt = I4FrameTimer::getFrameTimer()->updateFrameDelta();

			elapsedSec += dt;		

			while (elapsedSec >= I4FrameTimer::getFrameTimer()->getTickInterval())	// ����Ÿ�̹����� �ҷ����ϴ� �͵�. ex) ����..
			{
				elapsedSec -= I4FrameTimer::getFrameTimer()->getTickInterval();

				if (frameCallback)
				{
					I4FrameTimer::getFrameTimer()->updateTickCount();
					if (frameCallback->onTick(I4FrameTimer::getFrameTimer()->getTickInterval()) == false)
					{
						done = true;
					}
				}
			}

			if (frameCallback)
			{
				if (frameCallback->onUpdate() == false)
				{
					done = true;
				}

				if (frameCallback->onRender() == false)
				{
					done = true;
				}
			}

			++frameCount;
			static float elapsedLastDisplayFPS = 0;
			elapsedLastDisplayFPS += dt;
			if (elapsedLastDisplayFPS >= 1.0f)
			{
				fps = (float)frameCount/elapsedLastDisplayFPS;

				elapsedLastDisplayFPS = 0;
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