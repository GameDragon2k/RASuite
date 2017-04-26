#ifndef RA_DDRAW_H
#define RA_DDRAW_H

#ifndef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION         0x0500
#endif

#include <ddraw.h>

extern bool disable_RA_overlay;

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void CreateOverlay(HWND hwnd, HINSTANCE hInstance);
void RenderAchievementsOverlay(HWND hwnd);




#endif