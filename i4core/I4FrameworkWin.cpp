#include "stdafx.h"
#include "I4FrameworkWin.h"
#include "I4FrameCallback.h"
#include "I4StringUtil.h"
#include "I4Log.h"

namespace i4core {

	I4FrameworkWin* g_frameworkWin = nullptr;

	// 전역 메시지 프로시저
	LRESULT CALLBACK appWndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam)
	{
		// 전역 윈32 프레임워크 객체의 메시지 프로시저를 호출한다.
		return g_frameworkWin->wndProc(hWnd, iMsg, wParam, lParam);
	}

	I4FrameworkWin::I4FrameworkWin()
	: hInst(nullptr)
	{
		assert(g_frameworkWin == nullptr);	// 하나만 생성되어야 하기때문에 이미 할당되있으면 안된다.
		
		g_frameworkWin = this;
	}

	I4FrameworkWin::~I4FrameworkWin()
	{
		g_frameworkWin = nullptr;
	}

	bool I4FrameworkWin::onCreate()
	{
		hInst = GetModuleHandle(nullptr);

		wstring wtitle;
		to_wstring(wtitle, title);
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

	void I4FrameworkWin::onDestroy()
	{
		wstring wtitle;
		to_wstring(wtitle, title);
		UnregisterClass(wtitle.c_str(), hInst);
	}


	bool I4FrameworkWin::onMessagePump()
	{
		POINT pt;
		GetCursorPos(&pt);

		I4InputState::MouseX = pt.x;
		I4InputState::MouseY = pt.y;

		if (I4InputState::MoveMouseCenter)
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
	
	LRESULT CALLBACK I4FrameworkWin::wndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (iMsg)
		{
		case WM_KEYDOWN:
			{
				unsigned int key = (unsigned int)wParam;
				if (I4InputState::KeyPressed[key] == false)
				{
					I4InputState::KeyPressed[key] = true;

					if (frameCallback)
					{						
						I4InputState state;
						state.type = I4INPUT_KEY_DOWN;
						state.key = key;
						frameCallback->onInput(state);
					}
				}
			}
			break;
		case WM_KEYUP:
			{
				unsigned int key = (unsigned int)wParam;
				if (I4InputState::KeyPressed[key] == true)
				{
					I4InputState::KeyPressed[key] = false;

					if (frameCallback)
					{
						I4InputState state;
						state.type = I4INPUT_KEY_UP;
						state.key = key;
						frameCallback->onInput(state);
					}
				}
			}
			
			break;
		case WM_LBUTTONDOWN:
			{
				if (I4InputState::LeftMousePressed == false)
				{
					SetCapture(hWnd);

					I4InputState::LeftMousePressed = true;

					if (frameCallback)
					{
						I4InputState state;
						state.type = I4INPUT_LEFT_MOUSE_DOWN;
						state.key = -1;
						frameCallback->onInput(state);
					}
				}
				
			}
			break;
		case WM_LBUTTONUP:
			{
				if (I4InputState::LeftMousePressed == true)
				{
					SetCapture(nullptr);

					I4InputState::LeftMousePressed = false;

					if (frameCallback)
					{
						I4InputState state;
						state.type = I4INPUT_LEFT_MOUSE_UP;
						state.key = -1;
						frameCallback->onInput(state);
					}
				}
			}
			break;
		case WM_RBUTTONDOWN:
			{
				if (I4InputState::RightMousePressed == false)
				{
					I4InputState::RightMousePressed = true;

					SetCapture(hWnd);

					if (frameCallback)
					{
						I4InputState state;
						state.type = I4INPUT_RIGHT_MOUSE_DOWN;
						state.key = -1;
						frameCallback->onInput(state);
					}
				}
			}
			break;
		case WM_RBUTTONUP:
			{
				if (I4InputState::RightMousePressed == true)
				{
					I4InputState::RightMousePressed = false;

					SetCapture(nullptr);

					if (frameCallback)
					{
						I4InputState state;
						state.type = I4INPUT_RIGHT_MOUSE_UP;
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
				I4InputState::initialize();
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