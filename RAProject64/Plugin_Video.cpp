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

char GfxDLL[100];
HINSTANCE hGfxDll;

/********** GFX DLL: Functions *********************/
void (__cdecl *_CaptureScreen)      ( char * ) = NULL;
void (__cdecl *_ChangeWindow)       ( void ) = NULL;
void (__cdecl *_GFXCloseDLL)        ( void ) = NULL;
void (__cdecl *_GFXDllAbout)        ( HWND hParent ) = NULL;
void (__cdecl *_GFXDllConfig)       ( HWND hParent ) = NULL;
void (__cdecl *_GfxRomClosed)       ( void ) = NULL;
void (__cdecl *_GfxRomOpen)         ( void ) = NULL;
void (__cdecl *_DrawScreen)         ( void ) = NULL;
void (__cdecl *_FrameBufferRead)    ( DWORD addr ) = NULL;
void (__cdecl *_FrameBufferWrite)   ( DWORD addr, DWORD Bytes ) = NULL;
BOOL (__cdecl *_InitiateGFX)        ( GFX_INFO Gfx_Info ) = NULL;
void (__cdecl *_MoveScreen)         ( int xpos, int ypos ) = NULL;
void (__cdecl *_ProcessDList)       ( void ) = NULL;
void (__cdecl *_ProcessRDPList)     ( void ) = NULL;
void (__cdecl *_ShowCFB)			( void ) = NULL;
void (__cdecl *_UpdateScreen)       ( void ) = NULL;
void (__cdecl *_ViStatusChanged)    ( void ) = NULL;
void (__cdecl *_ViWidthChanged)     ( void ) = NULL;

BOOL LoadGFXDll(char * RspDll) {

	if(hGfxDll != NULL)
		CloseGfxPlugin();

	PLUGIN_INFO PluginInfo;
	char DllName[300];

	GetPluginDir(DllName);
	strcat(DllName,GfxDLL);

	hGfxDll = LoadLibrary(DllName);
	if (hGfxDll == NULL)
		return FALSE;

	GetDllInfo = (void (__cdecl *)(PLUGIN_INFO *))GetProcAddress( hGfxDll, "GetDllInfo" );
	if (GetDllInfo == NULL) 
		return FALSE;

	GetDllInfo(&PluginInfo);
	//if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }

	_GFXCloseDLL = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "CloseDLL" );
	_ChangeWindow = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "ChangeWindow" );
	_GFXDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hGfxDll, "DllAbout" );
	_GFXDllConfig = (void (__cdecl *)(HWND))GetProcAddress( hGfxDll, "DllConfig" );
	_DrawScreen = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "DrawScreen" );
	_InitiateGFX = (BOOL (__cdecl *)(GFX_INFO))GetProcAddress( hGfxDll, "InitiateGFX" );
	_MoveScreen = (void (__cdecl *)(int, int))GetProcAddress( hGfxDll, "MoveScreen" );
	_ProcessDList = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "ProcessDList" );
	_GfxRomClosed = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "RomClosed" );
	_GfxRomOpen = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "RomOpen" );
	_UpdateScreen = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "UpdateScreen" );
	_ViStatusChanged = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "ViStatusChanged" );
	_ViWidthChanged = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "ViWidthChanged" );
	
	_ProcessRDPList = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "ProcessRDPList" );
	_CaptureScreen = (void (__cdecl *)(char *))GetProcAddress( hGfxDll, "CaptureScreen" );
	_ShowCFB = (void (__cdecl *)(void))GetProcAddress( hGfxDll, "ShowCFB" );

#ifdef CFB_READ
	FrameBufferRead = (void (__cdecl *)(DWORD))GetProcAddress( hGfxDll, "FBRead" );
	FrameBufferWrite = (void (__cdecl *)(DWORD, DWORD))GetProcAddress( hGfxDll, "FBWrite" );
#endif
	return TRUE;
}

void CloseGfxPlugin()
{
	GfxRomClosed();
	GFXCloseDLL();

	if(hGfxDll)
	{
		FreeLibrary(hGfxDll);
		hGfxDll = NULL;
	}

	_CaptureScreen = NULL;
	_ChangeWindow = NULL;
	_GFXCloseDLL = NULL;
	_GFXDllAbout = NULL;
	_GFXDllConfig = NULL;
	_GfxRomClosed = NULL;
	_GfxRomOpen = NULL;
	_DrawScreen = NULL;
	_FrameBufferRead = NULL;
	_FrameBufferWrite = NULL;
	_InitiateGFX = NULL;
	_MoveScreen = NULL;
	_ProcessDList = NULL;
	_ProcessRDPList = NULL;
	_ShowCFB = NULL;
	_UpdateScreen = NULL;
	_ViStatusChanged = NULL;
	_ViWidthChanged = NULL;
}

//****************
// DLL FUNCTIONS
//***************

//Lets initiate the plugin
BOOL InitiateGFX(GFX_INFO Gfx_Info)
{
	if(_InitiateGFX != NULL)
	{
		__try
		{
			return _InitiateGFX(Gfx_Info);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
			//We failed to initialize graphics
		}
	}
	return false;
}

//Tell the DLL that we are closing it
void GFXCloseDLL()
{
	if(_GFXCloseDLL != NULL)
	{
		__try
		{
			_GFXCloseDLL();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in GFXCloseDLL");
		}
	}
}

//Alert the video plugin that a rom has been opened
void GfxRomOpen(void)
{
	if(_GfxRomOpen != NULL)
	{
		__try
		{
			_GfxRomOpen();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			//We failed to tell the plugin that a rom has been opend
		}
	}
}

//Tell the plugin the rom has been closed
void GfxRomClosed(void)
{
	if(_GfxRomClosed != NULL)
	{
		__try
		{
			_GfxRomClosed();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			//
		}
	}
}

//Load the interface for the about box
void GFXDllAbout( HWND hParent )
{
	if(_GFXDllAbout != NULL)
		_GFXDllAbout(hParent);
}

//Load the interface for the config
void GFXDllConfig( HWND hParent )
{
	if(_GFXDllConfig != NULL)
		_GFXDllConfig(hParent);
}

//
void ShowCFB()
{
	if(_ShowCFB != NULL)
	{
		__try
		{
			_ShowCFB();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in ShowCFB()");
		}
	}
}

//Tell the plugin to write bytes to the given address in the frame buffer
void FrameBufferWrite(DWORD addr, DWORD Bytes )
{
	if(_FrameBufferWrite != NULL)
	{
		__try
		{
			_FrameBufferWrite(addr, Bytes);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in FrameBufferWrite");
		}
	}
}

//Tell the plugin to read the frame buffer from the given address
void FrameBufferRead(DWORD addr)
{
	if(_FrameBufferRead != NULL)
	{
		__try
		{
			_FrameBufferRead(addr);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in FrameBufferRead");
		}
	}
}

//Tell the plugin to read the ViWidth again as it has been changed
void ViWidthChanged()
{
	if(_ViWidthChanged != NULL)
	{
		__try
		{
			_ViWidthChanged();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured in ViWidthChanged");
		}

	}
}

//tell the plugin to read vi status again as it has been changed
void ViStatusChanged()
{
	if(_ViStatusChanged != NULL)
	{
		__try
		{
			_ViStatusChanged();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured in ViStatusChanged.");
		}
	}
}

//Tell the plugin to update the screen
void UpdateScreen(void)
{
	if(_UpdateScreen != NULL)
	{
		__try
		{
			_UpdateScreen();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured when updating screen.");
		}
	}
}

//Tell the plugin to draw the screem
void DrawScreen(void)
{
	if(_DrawScreen != NULL)
	{
		__try
		{
			_DrawScreen();


		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception while drawing screen");
		}
	}
}

//Tell the video plugin that we are changing the window
void ChangeWindow(void)
{
	if(_ChangeWindow != NULL)
	{
		__try
		{
			_ChangeWindow();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured when changing window.");
		}
	}
}

//Tell the video plugin that the screen has moved
void MoveScreen( int xpos, int ypos)
{
	if(_MoveScreen != NULL)
	{
		__try
		{
			_MoveScreen(xpos, ypos);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured when moving the screen.");
		}
	}
}

//Capture a screenshot of the window
void CaptureScreen(char * Directory)
{
	if(_CaptureScreen != NULL)
	{
		__try
		{
			_CaptureScreen(Directory);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured when capturing the screen.");
		}
	}
}

//Tell the plugin to handle the RDP list
void ProcessRDPList(void)
{
	if(_ProcessRDPList != NULL)
	{
		__try
		{
			_ProcessRDPList();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured when processing RDP list.");
		}
	}
}
//Tell the video plugin to process the D list
void ProcessDList(void)
{
	if(_ProcessDList != NULL)
	{
		__try
		{
			_ProcessDList();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception occured when processing D list.");
		}
	}
}