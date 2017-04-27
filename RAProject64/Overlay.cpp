
#include <stdio.h>
#include <math.h>
#include "Main.h"
#include "Cpu.h"
#include "Plugin_Input.h"

#include "../RA_Integration/RA_Interface.h"

HWND layeredWnd;
RECT rect;

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_SIZE:
		return SIZE_RESTORED;
	case WM_NCHITTEST:
		return HTNOWHERE;

	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}

void CreateOverlay(HWND hwnd){


		//Set up window class
		WNDCLASSA wndEx;
		memset(&wndEx, 0, sizeof(wndEx));
		wndEx.cbWndExtra  = sizeof(wndEx);
		wndEx.lpszClassName = "RA_WND_CLASS";
		wndEx.lpfnWndProc = OverlayWndProc;
		wndEx.hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		int result = RegisterClass(&wndEx);

		// Create Window. WS_POPUP style is important so window displays without any borders or toolbar;
		layeredWnd = CreateWindowEx(
			(WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED),
			wndEx.lpszClassName,
			"RAWnd",
			(WS_POPUP),
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
			NULL, NULL, wndEx.hInstance, NULL);

		//SetParent(hwnd, layeredWnd);
		
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
	GetKeys(0, &Keys); // Get keys from 1st player controller.

	input.m_bUpPressed		= Keys.U_DPAD;
	input.m_bDownPressed	= Keys.D_DPAD;
	input.m_bLeftPressed	= Keys.L_DPAD;
	input.m_bRightPressed	= Keys.R_DPAD;
	//bAltButton = !Controller_1_A;
	input.m_bCancelPressed	= Keys.B_BUTTON;
	input.m_bConfirmPressed = Keys.A_BUTTON;
	input.m_bQuitPressed	= Keys.START_BUTTON;

	if (RA_OverlayStatus && (Keys.B_BUTTON || Keys.START_BUTTON))
		RA_OverlayStatus = FALSE;

	RA_UpdateRenderOverlay(hdc, &input, ((float)nDelta / 1000.0f), &rect, FALSE, (bool)CPU_Paused!=0);
}

void RenderAchievementsOverlay(HWND hwnd) {
	//#RA:
	//WARNING: Ugly Hack


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
		GetClientRect(hwnd, &rect);

		static HDC hdcMem = NULL;
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
}

