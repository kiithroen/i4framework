#include "I4FrameworkWin.h"
#include "I4FrameCallback.h"
#include "I4FrameStateMgr.h"
#include "I4StringUtil.h"
#include "I4Log.h"

namespace i4core {

	I4FrameworkWin* g_frameworkWin = NULL;

	// ���� �޽��� ���ν���
	LRESULT CALLBACK appWndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam)
	{
		// ���� ��32 �����ӿ�ũ ��ü�� �޽��� ���ν����� ȣ���Ѵ�.
		return g_frameworkWin->wndProc(hWnd, iMsg, wParam, lParam);
	}

	I4FrameworkWin::I4FrameworkWin()
	: hInst(NULL)
	{
		assert(g_frameworkWin == NULL);	// �ϳ��� �����Ǿ�� �ϱ⶧���� �̹� �Ҵ�������� �ȵȴ�.
		
		g_frameworkWin = this;
	}

	I4FrameworkWin::~I4FrameworkWin()
	{
		g_frameworkWin = NULL;
	}

	bool I4FrameworkWin::onCreate()
	{
		hInst = GetModuleHandle(NULL);

		const wchar_t* wtitle = I4StringUtil::to_wchar_t(title.c_str());
		// ������ Ŭ���� ���
		WNDCLASSEX	wc;

		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= appWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInst;
		wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= wtitle;
		wc.hIconSm			= LoadIcon(0, IDI_APPLICATION);

		if (RegisterClassEx(&wc) == 0)
			return false;

		// ������ ũ�� ����
		RECT rect = {0, 0, width, height};

		DWORD style = WS_OVERLAPPEDWINDOW;
		DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;

		AdjustWindowRectEx(&rect, style, false, exStyle);

		// ������ ����
		HWND hWnd = CreateWindowEx(
					exStyle,
					wtitle,
					wtitle,
					style,
					0,
					0,
					rect.right - rect.left,
					rect.bottom - rect.top,
					NULL,
					NULL,
					hInst,
					NULL);
		
		if (hWnd == NULL)
			return false;

		ShowWindow(hWnd, SW_SHOW);
		
		windowID = hWnd;

		return true;
	}

	void I4FrameworkWin::onDestroy()
	{
		const wchar_t* wtitle = I4StringUtil::to_wchar_t(title.c_str());
		UnregisterClass(wtitle, hInst);
	}


	bool I4FrameworkWin::onRun()
	{
		MSG msg;
		memset(&msg, 0, sizeof(MSG));
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return (msg.message != WM_QUIT);
	}

	void I4FrameworkWin::onYield()
	{
	//	Sleep(1);
	}

	void I4FrameworkWin::getMousePos(int& x, int& y)
	{
		POINT pt;
		GetCursorPos(&pt);

		x = pt.x;
		y = pt.y;
	}

	void I4FrameworkWin::moveMouseCenter()
	{
		// ���콺�� �������� �߾����� �ʱ�ȭ
		POINT pt;
		RECT rc;	
		GetClientRect((HWND)windowID, &rc);
		pt.x = (rc.right - rc.left)/2;
		pt.y = (rc.bottom - rc.top)/2;
		ClientToScreen((HWND)windowID, &pt);
		SetCursorPos(pt.x, pt.y);
	}

	LRESULT CALLBACK I4FrameworkWin::wndProc(HWND hWnd, unsigned int iMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (iMsg)
		{
		case WM_KEYDOWN:
			{
				unsigned int key = (unsigned int)wParam;
				if (keyPressed[key] == false)
				{
					keyPressed[key] = true;

					if (frameCallback)
					{
						frameCallback->onKeyDown(key);
					}
				}
			}
			break;
		case WM_KEYUP:
			{
				unsigned int key = (unsigned int)wParam;
				if (keyPressed[key] == true)
				{
					keyPressed[key] = false;

					if (frameCallback)
					{
						frameCallback->onKeyUp(key);
					}
				}
			}
			
			break;
		case WM_LBUTTONDOWN:
			{
				SetCapture(hWnd);

				if (frameCallback)
				{
					frameCallback->onLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				}
			}
			break;
		case WM_LBUTTONUP:
			{
				SetCapture(NULL);

				if (frameCallback)
				{
					frameCallback->onLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				}
			}
			break;
		case WM_RBUTTONDOWN:
			{
				SetCapture(hWnd);

				if (frameCallback)
				{
					frameCallback->onRButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				}
			}
			break;
		case WM_RBUTTONUP:
			{
				SetCapture(NULL);

				if (frameCallback)
				{
					frameCallback->onRButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				}
			}
			break;
		case WM_MOUSEMOVE:
			{
				if (frameCallback)
				{
					frameCallback->onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				}
			}
			break;
		case WM_ACTIVATEAPP:
			if (wParam == TRUE)
			{
				activated = true;
			}
			else
			{
				activated = false;
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