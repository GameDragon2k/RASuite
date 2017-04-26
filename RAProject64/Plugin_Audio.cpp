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
#include "main.h"
#include "Plugin.h"

char AudioDLL[100];
HINSTANCE hAudioDll;
HANDLE hAudioThread = NULL;


/************ Audio DLL: Functions *****************/
void (__cdecl *_AiCloseDLL)       ( void ) = NULL;
void (__cdecl *_AiDacrateChanged) ( int SystemType ) = NULL;
void (__cdecl *_AiLenChanged)     ( void ) = NULL;
void (__cdecl *_AiDllAbout)       ( HWND hParent ) = NULL;
void (__cdecl *_AiDllConfig)      ( HWND hParent ) = NULL;
void (__cdecl *_AiDllTest)        ( HWND hParent ) = NULL;
DWORD (__cdecl *_AiReadLength)    ( void ) = NULL;
void (__cdecl *_AiRomClosed)      ( void ) = NULL;
void (__cdecl *_AiUpdate)         ( BOOL Wait ) = NULL;
BOOL (__cdecl *_InitiateAudio)    ( AUDIO_INFO Audio_Info ) = NULL;
void (__cdecl *_ProcessAList)     ( void ) = NULL;

bool LoadAudioDll(char * AudioDll) {

	if(hAudioDll != NULL)
		FreeLibrary(hAudioDll);

	PLUGIN_INFO PluginInfo;
	char DllName[300];

	GetPluginDir(DllName);
	strcat(DllName,AudioDll);

	hAudioDll = LoadLibrary(DllName);
	if (hAudioDll == NULL) 
		return FALSE;


	GetDllInfo = (void (__cdecl *)(PLUGIN_INFO *))GetProcAddress( hAudioDll, "GetDllInfo" );
	if (GetDllInfo == NULL)
		return FALSE;

	GetDllInfo(&PluginInfo);

	//Should we even bother doing this?
	//if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }

	_AiCloseDLL = (void (__cdecl *)(void))GetProcAddress( hAudioDll, "CloseDLL" );
	_AiDacrateChanged = (void (__cdecl *)(int))GetProcAddress( hAudioDll, "AiDacrateChanged" );
	_AiLenChanged = (void (__cdecl *)(void))GetProcAddress( hAudioDll, "AiLenChanged" );
	_AiReadLength = (DWORD (__cdecl *)(void))GetProcAddress( hAudioDll, "AiReadLength" );
	_InitiateAudio = (BOOL (__cdecl *)(AUDIO_INFO))GetProcAddress( hAudioDll, "InitiateAudio" );
	_AiRomClosed = (void (__cdecl *)(void))GetProcAddress( hAudioDll, "RomClosed" );
	_ProcessAList = (void (__cdecl *)(void))GetProcAddress( hAudioDll, "ProcessAList" );	
	_AiDllConfig = (void (__cdecl *)(HWND))GetProcAddress( hAudioDll, "DllConfig" );
	_AiUpdate = (void (__cdecl *)(BOOL))GetProcAddress( hAudioDll, "AiUpdate" );
	_AiDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hAudioDll, "DllAbout");
	_AiDllTest = (void (__cdecl *)(HWND))GetProcAddress( hAudioDll, "DllTest");
	return TRUE;
}

void AudioThread (void)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );
	for(int a = 0; a < 100; a++)
		AiUpdate(TRUE);
}


//Close the video plugin
void CloseAudioPlugin(void)
{
	/* AUDIO_End(); */
	AiRomClosed();
	AiCloseDll();

	if(hAudioDll)
	{
		FreeLibrary(hAudioDll);
		hAudioDll = NULL;
	}

	_AiCloseDLL       = NULL;
	_AiDacrateChanged = NULL;
	_AiLenChanged     = NULL;
	_AiReadLength     = NULL;
	_AiUpdate         = NULL;
	_InitiateAudio    = NULL;
	_ProcessAList     = NULL;
	_AiRomClosed      = NULL;
}

/****************
**DLL FUNCTIONS**
*****************/
BOOL InitiateAudio(AUDIO_INFO Audio_Info)
{
	if(_InitiateAudio != NULL)
	{
		__try
		{
			return _InitiateAudio(Audio_Info);
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			// Lets catch any exceptions rather than crashing out
		}
	}
	return false; // We cant initiate audio if it doesnt exist

}

//Tell the audio dll to close/shutdown
void AiCloseDll()
{
	if(_AiCloseDLL != NULL)
	{
		__try
		{
			_AiCloseDLL();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			//Lets catch any exceptions rather than crashing out
		}
	}
}	

//Tell the audio dll the rom has been closed
void AiRomClosed()
{
	if(_AiRomClosed != NULL)
	{
		__try
		{
			_AiRomClosed();
		}
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			//Lets catch any exceptions rather than crashing out
		}
	}
}

void ProcessAList()
{
	if(_ProcessAList != NULL)
		_ProcessAList();
}

void AiDacrateChanged(int SystemType)
{
	if(_AiDacrateChanged !=NULL)
		_AiDacrateChanged(SystemType);
	//else
		//Do nothing
}

void AiLenChanged()
{
	if(_AiLenChanged != NULL)
		_AiLenChanged();
}

DWORD AiReadLength()
{
	if(_AiReadLength != NULL)
		return _AiReadLength();
	else
		return 0;
}

void AiUpdate( BOOL update )
{
	//if(_AiUpdate != NULL)
		//AiUpdate(update);
}

void AiDllConfig( HWND hParent)
{
	if(_AiDllConfig != NULL)
		_AiDllConfig(hParent);

}	

void AiDllAbout( HWND hParent)
{
	if(_AiDllAbout != NULL)
		_AiDllAbout(hParent);
}

void AiDllTest( HWND hParent)
{
	if(_AiDllTest != NULL)
		_AiDllTest(hParent);
}