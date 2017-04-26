/*
 * Project 64 - A Nintendo 64 emulator.
 *
 * (c) Copyright 2001 zilmar (zilmar@emulation64.com) and 
 * Jabo (jabo@emulation64.com).
 *
 * pj64 homepage: www.pj64.net
 *
 * Permission to use, copy, modify and distribute Project64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Project64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Project64 or software derived from Project64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */
#include "Main.h"
#include "Plugin.h"

char ControllerDLL[100];
CONTROL Controllers[4];
HINSTANCE hControllerDll;
WORD ContVersion;

/********** Controller DLL: Functions **************/
void (__cdecl *_ContCloseDLL)     ( void ) = NULL;
void (__cdecl *_ControllerCommand)( int Control, BYTE * Command ) = NULL;
void (__cdecl *_ContDllAbout)     ( HWND hParent ) = NULL;
void (__cdecl *_ContConfig)       ( HWND hParent ) = NULL;
void (__cdecl *_InitiateControllers_1_0)( HWND hMainWindow, CONTROL Controls[4] ) = NULL;
void (__cdecl *_InitiateControllers_1_1)( CONTROL_INFO ControlInfo ) = NULL;
void (__cdecl *_GetKeys)          ( int Control, BUTTONS * Keys ) = NULL;
void (__cdecl *_ReadController)   ( int Control, BYTE * Command ) = NULL;
void (__cdecl *_ContRomOpen)      ( void ) = NULL;
void (__cdecl *_ContRomClosed)    ( void ) = NULL;
void (__cdecl *_WM_KeyDown)       ( WPARAM wParam, LPARAM lParam ) = NULL;
void (__cdecl *_WM_KeyUp)         ( WPARAM wParam, LPARAM lParam ) = NULL;
void (__cdecl *_RumbleCommand)	 ( int Control, BOOL bRumble ) = NULL;

BOOL LoadInputDll(char * InputDll) {

	if(hControllerDll)
		CloseInputPlugin();
	
	PLUGIN_INFO PluginInfo;
	char DllName[300];

	GetPluginDir(DllName);
	strcat(DllName,InputDll);

	hControllerDll = LoadLibrary(DllName);
	if (hControllerDll == NULL) 
		return FALSE;

	GetDllInfo = (void (__cdecl *)(PLUGIN_INFO *))GetProcAddress( hControllerDll, "GetDllInfo" );
	if (GetDllInfo == NULL)
		return FALSE;

	GetDllInfo(&PluginInfo);
	ContVersion = PluginInfo.Version;

	_ContCloseDLL = (void (__cdecl *)(void))GetProcAddress( hControllerDll, "CloseDLL" );

	if (ContVersion == 0x0100)
		_InitiateControllers_1_0 = (void (__cdecl *)(HWND, CONTROL *))GetProcAddress( hControllerDll, "InitiateControllers" );

	if (ContVersion == 0x0101)
		_InitiateControllers_1_1 = (void (__cdecl *)(CONTROL_INFO))GetProcAddress( hControllerDll, "InitiateControllers" );

	_ControllerCommand = (void (__cdecl *)(int, BYTE *))GetProcAddress( hControllerDll, "ControllerCommand" );
	_ReadController = (void (__cdecl *)(int, BYTE *))GetProcAddress( hControllerDll, "ReadController" );
	_ContConfig = (void (__cdecl *)(HWND))GetProcAddress( hControllerDll, "DllConfig" );
	_ContDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hControllerDll, "DllAbout" );
	_GetKeys = (void (__cdecl *)(int, BUTTONS *))GetProcAddress( hControllerDll, "GetKeys" );
	_WM_KeyDown = (void (__cdecl *)(WPARAM,LPARAM))GetProcAddress( hControllerDll, "WM_KeyDown" );
	_WM_KeyUp = (void (__cdecl *)(WPARAM,LPARAM))GetProcAddress( hControllerDll, "WM_KeyUp" );

	_ContRomOpen = (void (__cdecl *)(void))GetProcAddress( hControllerDll, "RomOpen" );
	_ContRomClosed = (void (__cdecl *)(void))GetProcAddress( hControllerDll, "RomClosed" );

	_RumbleCommand = (void (__cdecl *)(int, BOOL))GetProcAddress( hControllerDll, "RumbleCommand" );
	return TRUE;
}

void CloseInputPlugin()
{
	ContRomClosed();
	ContCloseDLL();
	if(hControllerDll)
		FreeLibrary(hControllerDll);

	_ContCloseDLL = NULL;
	_ControllerCommand = NULL;
	_ContDllAbout = NULL;
	_ContConfig = NULL;
	_InitiateControllers_1_0 = NULL;
	_InitiateControllers_1_1 = NULL;
	_GetKeys = NULL;
	_ReadController = NULL;
	_ContRomOpen = NULL;
	_ContRomClosed = NULL;
	_WM_KeyDown = NULL;
	_WM_KeyUp = NULL;
	_RumbleCommand = NULL;
}
/****************
**DLL FUNCTIONS**
*****************/
void InitiateControllers_1_0(HWND hMainWindow, CONTROL Controls[4] )
{
	if(_InitiateControllers_1_0 != NULL)
	{
		__try
		{
			_InitiateControllers_1_0(hMainWindow, Controls);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void InitiateControllers_1_1(CONTROL_INFO ControlInfo)
{
	if(_InitiateControllers_1_1 != NULL)
	{
		__try
		{
			_InitiateControllers_1_1(ControlInfo);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void ContCloseDLL()
{
	if(_ContCloseDLL != NULL)
	{
		__try
		{
			_ContCloseDLL();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void ContRomOpen()
{
	if(_ContRomOpen != NULL)
	{
		__try
		{
			_ContRomOpen();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void ContRomClosed()
{
	if(_ContRomClosed != NULL)
	{
		__try
		{
			_ContRomClosed();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void ContDllAbout(HWND hParent)
{
	if(_ContDllAbout != NULL)
	{
		__try
		{
			_ContDllAbout(hParent);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void ContConfig(HWND hParent)
{
	if(_ContConfig != NULL)
	{
		__try
		{
			_ContConfig(hParent);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void ControllerCommand(int Control, BYTE * Command )
{
	if(_ControllerCommand != NULL)
	{
		__try
		{
			_ControllerCommand(Control, Command );
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void ReadController(int Control, BYTE * Command )
{
	if(_ReadController != NULL)
	{
		__try
		{
			_ReadController(Control, Command );
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void GetKeys(int Control, BUTTONS * Keys )
{
	if(_GetKeys != NULL)
	{
		__try
		{
			_GetKeys(Control, Keys );
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void RumbleCommand( int Control, BOOL bRumble )
{
	if(_RumbleCommand != NULL)
	{
		__try
		{
			_RumbleCommand( Control, bRumble );
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void WM_KeyUp( WPARAM wParam, LPARAM lParam )
{
	if(_WM_KeyUp != NULL)
	{
		__try
		{
			_WM_KeyUp( wParam, lParam );
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void WM_KeyDown( WPARAM wParam, LPARAM lParam )
{
	if(_WM_KeyDown != NULL)
	{
		__try
		{
			_WM_KeyDown( wParam, lParam );
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}