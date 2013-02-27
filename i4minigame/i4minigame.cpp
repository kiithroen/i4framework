// i4minigame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "i4minigame.h"
#include "I4Log.h"
#include "I4Framework.h"
#include "I4MiniGameFrameCallback.h"
using namespace i4core;

#ifdef _DEBUG
#include <vld.h>
#endif

using namespace i4core;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
#ifdef _DEBUG
	I4Log::initialize(I4Log::FLAG_DEBUGGER|I4Log::FLAG_FILE, I4Log::LEVEL_DEBUG, L"i4minigame_d.log");
#else
	I4Log::initialize(I4Log::FLAG_DEBUGGER|I4Log::FLAG_FILE, I4Log::LEVEL_DEBUG, L"i4minigame.log");
#endif	

	I4MiniGameFrameCallback frameCallback;
	I4Framework::createFramework();
	I4Framework* framework = I4Framework::getFramework();
	framework->setFrameCallback(&frameCallback);
	if (framework->create(1366, 768, "i4 mini game") == false)
	{
		I4LOG_ERROR << "Framework create failed.\n";
		return false;
	}
	framework->run();
	I4Framework::destroyFramework();
	return 0;
}