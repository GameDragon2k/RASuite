#include <windows.h>
#include <EXDISP.h>
#include "main.h"
#include "resource.h"

LRESULT CALLBACK AboutBoxProc    ( HWND, UINT, WPARAM, LPARAM );

void AboutBox (void) {
	DialogBox(hInst, MAKEINTRESOURCE(IDD_About), hMainWindow, (DLGPROC)AboutBoxProc);
}

LRESULT CALLBACK AboutBoxProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			CoInitialize(NULL);

					
			HWND m_hwndWebBrowser;
			

			DWORD dwStyle = ::GetWindowLong(m_hwndWebBrowser, GWL_STYLE);
			DWORD dwNewStyle = (dwStyle & ~(WS_BORDER|WS_THICKFRAME|WS_SYSMENU|WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX) | WS_CHILD);
			::SetWindowLong(m_hwndWebBrowser, GWL_STYLE, dwNewStyle);

			DWORD dwStyle2 = ::GetWindowLong(m_hwndWebBrowser, GWL_EXSTYLE);
			DWORD dwNewStyle2 = (dwStyle2 & ~WS_EX_NOPARENTNOTIFY);
			::SetWindowLong(m_hwndWebBrowser, GWL_EXSTYLE, dwNewStyle2);

			RECT rc;
			GetWindowRect(GetDlgItem(hDlg,IDC_IE_CONTROL),&rc);
			MapWindowPoints(NULL,hDlg,(LPPOINT)&rc,2);
			SetWindowPos(m_hwndWebBrowser,HWND_TOP,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,SWP_SHOWWINDOW);

			SetParent( m_hwndWebBrowser, hDlg );

			BringWindowToTop(m_hwndWebBrowser);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:

			CoUninitialize();
			EndDialog(hDlg,0);
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}
