// i4minigame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "i4minigame.h"
#include "I4Log.h"
#include "I4Framework.h"
#include "I4MiniGameFrameCallback.h"
using namespace i4core;

#ifdef _DEBUG
#//include <vld.h>
#endif

using namespace i4core;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(437);
#ifdef _DEBUG
	Log::initialize(Log::FLAG_DEBUGGER|Log::FLAG_FILE, Log::LEVEL_DEBUG, L"i4minigame_d.log");
#else
	Log::initialize(Log::FLAG_DEBUGGER|Log::FLAG_FILE, Log::LEVEL_DEBUG, L"i4minigame.log");
#endif	

	MiniGameFrameCallback frameCallback;
	Framework::createFramework();
	Framework* framework = Framework::getFramework();
	framework->setFrameCallback(&frameCallback);
	if (framework->create(1366, 768, "i4 mini game") == false)
	{
		LOG_ERROR << "Framework create failed.\n";
		return false;
	}
	framework->run();
	Framework::destroyFramework();
	return 0;
}