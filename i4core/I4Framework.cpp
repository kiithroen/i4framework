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

	int	 InputState::MouseX;
	int	 InputState::MouseY;
	bool InputState::KeyPressed[256];
	bool InputState::LeftMousePressed;
	bool InputState::RightMousePressed;
	bool InputState::MoveMouseCenter;

	void InputState::initialize()
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


	Framework::Framework()
		: windowID(nullptr)
		, title("Untitled")
		, width(800)
		, height(600)
		, fps(0)
		, frameCallback(nullptr)
	{
		InputState::initialize();
	}

	Framework::~Framework()
	{
		destroy();
	}

	bool Framework::create(unsigned int _width, unsigned int _height, const string& _title)
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

		FrameTimer::createFrameTimer(TICK_INTERVAL);
		
		if (onCreate() == false)
		{
			destroy();
			return false;
		}

		return true;
	}

	void Framework::destroy()
	{
		FrameTimer::destroyFrameTimer();

		onDestroy();
	}

	void Framework::run()
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

		FrameTimer::getFrameTimer()->updateFrameDelta();

		bool done = false;
		while (!done)
		{
			if (onMessagePump() == false)
			{
				done = true;
			}

			float dt = FrameTimer::getFrameTimer()->updateFrameDelta();

			elapsedSec += dt;		

			while (elapsedSec >= FrameTimer::getFrameTimer()->getTickInterval())	// ����Ÿ�̹����� �ҷ����ϴ� �͵�. ex) ����..
			{
				elapsedSec -= FrameTimer::getFrameTimer()->getTickInterval();

				if (frameCallback)
				{
					FrameTimer::getFrameTimer()->updateTickCount();
					if (frameCallback->onTick(FrameTimer::getFrameTimer()->getTickInterval()) == false)
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

				ProfileWriterLog	writer;
				writer.write(ProfileManager::getRootNode(), fps);

				ProfileManager::clear();
			}
		}

		if (frameCallback)
		{
			frameCallback->onEnd();
		}
	}
	
	void Framework::setFrameCallback(FrameCallback* callback)
	{
		frameCallback = callback;
	}

	//-----------------------------------------------------------------------------

	Framework* Framework::framework = nullptr;

	void Framework::createFramework()
	{
		assert(framework == nullptr);

#ifdef _WIN32
		framework = new FrameworkWin;
#else
		LOG_WARN << "not implement yet...";
		assert(false);
		framework = new Framework;		
#endif	
	}

	void Framework::destroyFramework()
	{
		delete framework;
		framework = nullptr;
	}

}