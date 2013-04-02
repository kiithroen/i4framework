#pragma once

#include "i4core.h"
#include "I4Framework.h"

namespace i4core
{
	class FrameworkWin : public Framework
	{
		friend class Framework;
	public:
		virtual ~FrameworkWin();

	public:
		virtual bool		onCreate();
		virtual void		onDestroy();
		virtual bool		onMessagePump();

	public:
		LRESULT CALLBACK	wndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam);

	private:
		bool				mainLoop(float deltaMs);

	private:
		FrameworkWin();

	private:
		HINSTANCE		hInst;
	};
}