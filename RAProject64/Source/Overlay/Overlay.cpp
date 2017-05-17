
#include <stdafx.h>
#include <stdio.h>
#include <math.h>
#include <gl\GL.h>
//#include <WinGDI.h>
#include <Project64-core/Plugins/ControllerPlugin.h>
#include <Project64-core/N64System/SystemGlobals.h>

#include "../../RA_Integration/RA_Interface.h"

HWND layeredWnd;
RECT rect;
RECT swrect;
HBRUSH hbrush = CreateSolidBrush(0x110011);

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

void CreateOverlay(HWND hwnd)
{
		//Set up window class
		WNDCLASSA wndEx;
		memset(&wndEx, 0, sizeof(wndEx));
		wndEx.cbWndExtra  = sizeof(wndEx);
		wndEx.lpszClassName = "RA_WND_CLASS";
		wndEx.lpfnWndProc = OverlayWndProc;
		wndEx.hbrBackground = NULL;
		wndEx.hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		int result = RegisterClass(&wndEx);

		// Create Window. WS_POPUP style is important so window displays without any borders or toolbar;
		layeredWnd = CreateWindowEx(
			(WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED),
			wndEx.lpszClassName,
			"RAWnd",
			(WS_POPUP),
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
			hwnd, NULL, wndEx.hInstance, NULL);

		SetParent(hwnd, layeredWnd);
		
		ShowWindow(layeredWnd, SW_SHOWNOACTIVATE);
}

void UpdateOverlay(HDC hdc, RECT rect)
{
	static int nOldTime = GetTickCount(); //Time in ms I presume

	int nDelta;
	nDelta = GetTickCount() - nOldTime;
	nOldTime = GetTickCount();

	ControllerInput input; // This has to be passed to the overlay

	BUTTONS Keys;
	g_Plugins->Control()->GetKeys(0, &Keys); // Get keys from 1st player controller.

	input.m_bUpPressed		= Keys.U_DPAD;
	input.m_bDownPressed	= Keys.D_DPAD;
	input.m_bLeftPressed	= Keys.L_DPAD;
	input.m_bRightPressed	= Keys.R_DPAD;
	input.m_bCancelPressed	= Keys.B_BUTTON;
	input.m_bConfirmPressed = Keys.A_BUTTON;
	input.m_bQuitPressed	= Keys.START_BUTTON;

	//bool paused = g_Settings->LoadBool(GameRunning_CPU_Paused);

	RA_UpdateRenderOverlay(hdc, &input, ((float)nDelta / 1000.0f), &rect, 0, 0);
}

void RenderAchievementsOverlay(HWND hwnd, HWND status)
{
	// Set up buffer and back buffer
	if (layeredWnd == NULL)
	{
		GetClientRect(hwnd, &rect);
		GetClientRect(status, &swrect);
		rect.bottom -= swrect.bottom;
		CreateOverlay(hwnd);
	}
	else
	{
		GetClientRect(hwnd, &rect);
		GetClientRect(status, &swrect);
		rect.bottom -= swrect.bottom;
		HDC hdc = GetDC(layeredWnd);
		FillRect(hdc, &rect, hbrush);

		if (g_Settings->LoadBool(GameRunning_CPU_Running) || g_Settings->LoadBool(GameRunning_CPU_Paused))
			UpdateOverlay(hdc, rect);

		// Get position of the client rect. (NOT the window rect)
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rect.left));
		//ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rect.right));

		// Move layered window over MainWnd.
		MoveWindow(layeredWnd, rect.left, rect.top, rect.right, rect.bottom, FALSE);
		//SetWindowPos(layeredWnd, 0, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		//SetWindowPos(hwnd, layeredWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); // Don't think this line is necessary on most OS, but just safety net.

		ReleaseDC(layeredWnd, hdc);
	}
}

/*
void RenderAchievementsOverlay(HWND hwnd) {
	//#RA:
	//WARNING: Ugly Hack

	return;

	char currDir[2048];
	GetCurrentDirectory(2048, currDir); // "where'd you get the multithreaded code, Ted?"

	// Set up buffer and back buffer
	if (layeredWnd == NULL)
	{
		CreateOverlay(hwnd);
	}
	else
	{
		HDC hdc = GetDC(hwnd);
		static HDC hdcMem = NULL;
		GetClientRect(hwnd, &rect);

		//Set Pixel Format
		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));      // set the pixel format for the DC
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 24;
		pfd.iLayerType = PFD_MAIN_PLANE;
		SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
		SetPixelFormat(hdcMem, ChoosePixelFormat(hdcMem, &pfd), &pfd);

		HGLRC hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);

		static HBITMAP hBmp = NULL;
		static HBITMAP hBmpOld = NULL;
		if (!hdcMem) {
			hdcMem = CreateCompatibleDC(hdc);
			hBmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			hBmpOld = (HBITMAP)SelectObject(hdcMem, hBmp);
		}

		// Blits the MainWND to the back buffer.
		BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);

		// Update RA stuff
		UpdateOverlay(hdcMem, rect);

		// Actually draw to the back buffer
		// Not familiar with BLENDFUNCTION, may not be needed.
		BLENDFUNCTION blend = { 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = 255;
		blend.AlphaFormat = AC_SRC_OVER;
		POINT ptSrc = { 0, 0 };
		SIZE sizeWnd = { rect.right, rect.bottom };
		UpdateLayeredWindow(layeredWnd, hdc, NULL, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

		// Get position of the client rect. (NOT the window rect)
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rect.left));
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rect.right));

		// Move layered window over MainWnd.
		SetWindowPos(layeredWnd, 0, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		SetWindowPos(hwnd, layeredWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); // Don't think this line is necessary on most OS, but just safety net.

		ReleaseDC(hwnd, hdc);
	}
	

	SetCurrentDirectory(currDir); // "Cowboys Ted! They're a bunch of cowboys!"
}*/

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (Msg)
	{
	case WM_CREATE:
		SetLayeredWindowAttributes(hWnd, 0x110011, 255, LWA_COLORKEY);
		break;
	case WM_SIZE:
		return SIZE_RESTORED;
	case WM_NCHITTEST:
		return HTNOWHERE;

	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}
