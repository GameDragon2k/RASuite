
#include <stdio.h>
#include <math.h>
#include "Main.h"
#include "Cpu.h"


#include "../RA_Integration/RA_Interface.h"

HWND layeredWnd;
HWND MainWND;
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

void CreateOverlay(HWND hwnd, HINSTANCE hInstance){


		//Set up window class
		WNDCLASSA wndEx;
		memset(&wndEx, 0, sizeof(wndEx));
		wndEx.cbWndExtra  = sizeof(wndEx);
		wndEx.lpszClassName = "RA_WND_CLASS";
		wndEx.lpfnWndProc = OverlayWndProc;
		wndEx.hInstance = hInstance;
		int result = RegisterClass(&wndEx);

		// Create Window. WS_POPUP style is important so window displays without any borders or toolbar;
		hwnd =  CreateWindow("OverlayWnd",        
                        TEXT("RA Overlay"), 
                        WS_CHILD | WS_VISIBLE | WS_CAPTION 
                        | WS_SYSMENU | WS_THICKFRAME 
                        | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,  
                        CW_USEDEFAULT,      
                        CW_USEDEFAULT,      
                        300,        
                        300,        
                        layeredWnd,               
                        NULL,               
                        hInstance,          
                        NULL);  

		SetParent(hwnd, layeredWnd);
		
		ShowWindow(layeredWnd, SW_SHOWNORMAL);
		UpdateWindow(layeredWnd);

}

void UpdateOverlay(HDC hdc, RECT rect)
{


	static int nOldTime = GetTickCount(); //Time in ms I presume

	int nDelta;
	nDelta = GetTickCount() - nOldTime;
	nOldTime = GetTickCount();

	ControllerInput input; // This has to be passed to the overlay

	input.m_bUpPressed		= GetAsyncKeyState( VK_UP );
	input.m_bDownPressed	= GetAsyncKeyState( VK_DOWN);
	input.m_bLeftPressed	= GetAsyncKeyState( VK_LEFT );
	input.m_bRightPressed	= GetAsyncKeyState( VK_RIGHT );
	//bAltButton = !Controller_1_A;
	input.m_bCancelPressed	= GetAsyncKeyState( VK_ESCAPE );
	input.m_bConfirmPressed = GetAsyncKeyState( VK_SPACE );
	input.m_bQuitPressed	= GetAsyncKeyState( 0x50);


	RA_UpdateRenderOverlay(hdc, &input, ((float)nDelta / 1000.0f), &rect, AutoFullScreen, (bool)CPU_Paused);
}

void RenderAchievementsOverlay(HWND hwnd) {
	//#RA:
	//WARNING: Ugly Hack

	MainWND = hwnd;

	
	GetClientRect(hwnd, &rect);

	char currDir[2048];
	GetCurrentDirectory(2048, currDir); // "where'd you get the multithreaded code, Ted?"

	// Set up buffer and back buffer
		HDC hdc = GetDC(hwnd);

		static HDC hdcMem = NULL;
		static HBITMAP hBmp = NULL;
		static HBITMAP hBmpOld = NULL;
		if (!hdcMem) {
			hdcMem = CreateCompatibleDC(hdc);
			hBmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			hBmpOld = (HBITMAP)SelectObject(hdcMem, hBmp);
		}

		// Blits the MainWND to the back buffer.
		//BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);

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
		//UpdateLayeredWindow(layeredWnd, hdc, NULL, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
		UpdateWindow(hwnd);

		// Get position of the client rect. (NOT the window rect)
		//ClientToScreen(MainWND, reinterpret_cast<POINT*>(&rect.left));
		//ClientToScreen(MainWND, reinterpret_cast<POINT*>(&rect.right));

		// Move layered window over MainWnd.
		SetWindowPos(hwnd, 0, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		//SetWindowPos(MainWND, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); // Don't think this line is necessary on most OS, but just safety net.

		//SelectObject(hdcMem, hBmpOld);
		//DeleteObject(hBmp);
		//DeleteDC(hdcMem);
		ReleaseDC(hwnd, hdc);
	

	SetCurrentDirectory(currDir); // "Cowboys Ted! They're a bunch of cowboys!"
}

