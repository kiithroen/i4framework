#pragma once

#include "i4core.h"
#include "I4Framework.h"

namespace i4core
{
	class I4FrameworkWin : public I4Framework
	{
		friend class I4Framework;
	public:
		virtual ~I4FrameworkWin();

	public:
		virtual bool		onCreate();
		virtual void		onDestroy();
		virtual bool		onMessagePump();

	public:
		LRESULT CALLBACK	wndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam);

	private:
		bool				mainLoop(float deltaMs);

	private:
		I4FrameworkWin();

	private:
		HINSTANCE		hInst;
	};
}