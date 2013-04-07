#include "stdafx.h"
#include "I4FrameworkWin.h"
#include "I4FrameCallback.h"
#include "I4StringUtil.h"
#include "I4Log.h"

namespace i4core {

	FrameworkWin* g_frameworkWin = nullptr;

	// 전역 메시지 프로시저
	LRESULT CALLBACK appWndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam)
	{
		// 전역 윈32 프레임워크 객체의 메시지 프로시저를 호출한다.
		return g_frameworkWin->wndProc(hWnd, iMsg, wParam, lParam);
	}

	FrameworkWin::FrameworkWin()
	: hInst(nullptr)
	{
		assert(g_frameworkWin == nullptr);	// 하나만 생성되어야 하기때문에 이미 할당되있으면 안된다.
		
		g_frameworkWin = this;
	}

	FrameworkWin::~FrameworkWin()
	{
		g_frameworkWin = nullptr;
	}

	bool FrameworkWin::onCreate()
	{
		hInst = GetModuleHandle(nullptr);

		wstring wtitle;
		StringUtil::toWString(wtitle, title);
		// 윈도우 클래스 등록
		WNDCLASSEX	wc;

		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= appWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInst;
		wc.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName		= nullptr;
		wc.lpszClassName	= wtitle.c_str();
		wc.hIconSm			= LoadIcon(0, IDI_APPLICATION);

		if (RegisterClassEx(&wc) == 0)
			return false;

		// 윈도우 크기 설정
		RECT rect = {0, 0, width, height};

		DWORD style = WS_OVERLAPPEDWINDOW;
		DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;

		AdjustWindowRectEx(&rect, style, false, exStyle);

		// 윈도우 생성
		HWND hWnd = CreateWindowEx(
					exStyle,
					wtitle.c_str(),
					wtitle.c_str(),
					style,
					0,
					0,
					rect.right - rect.left,
					rect.bottom - rect.top,
					nullptr,
					nullptr,
					hInst,
					nullptr);
		
		if (hWnd == nullptr)
			return false;

		ShowWindow(hWnd, SW_SHOW);
		
		windowID = hWnd;

		return true;
	}

	void FrameworkWin::onDestroy()
	{
		wstring wtitle;
		StringUtil::toWString(wtitle, title);
		UnregisterClass(wtitle.c_str(), hInst);
	}


	bool FrameworkWin::onMessagePump()
	{
		POINT pt;
		GetCursorPos(&pt);

		InputState::MouseX = pt.x;
		InputState::MouseY = pt.y;

		if (InputState::MoveMouseCenter)
		{
			// 마우스를 윈도우의 중앙으로 초기화
			POINT pt;
			RECT rc;	
			GetClientRect((HWND)windowID, &rc);
			pt.x = (rc.right - rc.left)/2;
			pt.y = (rc.bottom - rc.top)/2;
			ClientToScreen((HWND)windowID, &pt);
			SetCursorPos(pt.x, pt.y);
		}

		MSG msg;
		memset(&msg, 0, sizeof(MSG));
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return (msg.message != WM_QUIT);
	}
	
	LRESULT CALLBACK FrameworkWin::wndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (iMsg)
		{
		case WM_KEYDOWN:
			{
				unsigned int key = (unsigned int)wParam;
				if (InputState::KeyPressed[key] == false)
				{
					InputState::KeyPressed[key] = true;

					if (frameCallback)
					{						
						InputState state;
						state.type = INPUT_KEY_DOWN;
						state.key = key;
						frameCallback->onInput(state);
					}
				}
			}
			break;
		case WM_KEYUP:
			{
				unsigned int key = (unsigned int)wParam;
				if (InputState::KeyPressed[key] == true)
				{
					InputState::KeyPressed[key] = false;

					if (frameCallback)
					{
						InputState state;
						state.type = INPUT_KEY_UP;
						state.key = key;
						frameCallback->onInput(state);
					}
				}
			}
			
			break;
		case WM_LBUTTONDOWN:
			{
				if (InputState::LeftMousePressed == false)
				{
					SetCapture(hWnd);

					InputState::LeftMousePressed = true;

					if (frameCallback)
					{
						InputState state;
						state.type = INPUT_LEFT_MOUSE_DOWN;
						state.key = -1;
						frameCallback->onInput(state);
					}
				}
				
			}
			break;
		case WM_LBUTTONUP:
			{
				if (InputState::LeftMousePressed == true)
				{
					SetCapture(nullptr);

					InputState::LeftMousePressed = false;

					if (frameCallback)
					{
						InputState state;
						state.type = INPUT_LEFT_MOUSE_UP;
						state.key = -1;
						frameCallback->onInput(state);
					}
				}
			}
			break;
		case WM_RBUTTONDOWN:
			{
				if (InputState::RightMousePressed == false)
				{
					InputState::RightMousePressed = true;

					SetCapture(hWnd);

					if (frameCallback)
					{
						InputState state;
						state.type = INPUT_RIGHT_MOUSE_DOWN;
						state.key = -1;
						frameCallback->onInput(state);
					}
				}
			}
			break;
		case WM_RBUTTONUP:
			{
				if (InputState::RightMousePressed == true)
				{
					InputState::RightMousePressed = false;

					SetCapture(nullptr);

					if (frameCallback)
					{
						InputState state;
						state.type = INPUT_RIGHT_MOUSE_UP;
						state.key = -1;
						frameCallback->onInput(state);
					}
				}
			}
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_ACTIVATEAPP:
			if (wParam == TRUE)
			{
				activated = true;
			}
			else
			{
				activated = false;
				InputState::initialize();
			}
			break;
		case WM_CLOSE:
			{
				PostQuitMessage(0);
			}
			
			break;

		}

		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
}