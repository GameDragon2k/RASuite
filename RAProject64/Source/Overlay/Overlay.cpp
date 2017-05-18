
#include <stdafx.h>
#include <stdio.h>
#include <math.h>
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
			"RAP64",
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

		// Move layered window over MainWnd.
		MoveWindow(layeredWnd, rect.left, rect.top, rect.right, rect.bottom, FALSE);

		ReleaseDC(layeredWnd, hdc);
	}
}

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
