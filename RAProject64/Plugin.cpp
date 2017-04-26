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
#include <windows.h>
#include <stdio.h>
#include "main.h"
#include "CPU.h"
#include "Debugger.h"
#include "plugin.h"
#include "settings.h"
#include "resource.h"

//plugin function pointers
void (__cdecl *GetDllInfo)             ( PLUGIN_INFO * PluginInfo );

/********** RSP DLL: Functions *********************/
void (__cdecl *GetRspDebugInfo)    ( RSPDEBUG_INFO * DebugInfo );
void (__cdecl *RSPCloseDLL)        ( void );
void (__cdecl *RSPDllAbout)        ( HWND hWnd );
void (__cdecl *RSPDllConfig)       ( HWND hWnd );
void (__cdecl *RSPRomClosed)       ( void );
DWORD (__cdecl *DoRspCycles)       ( DWORD );
void (__cdecl *InitiateRSP_1_0)    ( RSP_INFO_1_0 Rsp_Info, DWORD * Cycles);
void (__cdecl *InitiateRSP_1_1)    ( RSP_INFO_1_1 Rsp_Info, DWORD * Cycles);
void (__cdecl *InitiateRSPDebugger)( DEBUG_INFO DebugInfo);

//--------------------------------------

char RspDLL[100], * PluginNames[MaxDlls];
HINSTANCE hRspDll;
DWORD PluginCount, RspTaskValue, AudioIntrReg;
WORD RSPVersion;
RSPDEBUG_INFO RspDebug;
BOOL PluginsInitilized = FALSE;

BOOL PluginsChanged ( HWND hDlg );
BOOL ValidPluginVersion ( PLUGIN_INFO * PluginInfo );

void GetCurrentDlls (void)
{
	long lResult;
	HKEY hKeyResults = 0;
	char String[200];

	sprintf(String,"Software\\N64 Emulation\\%s\\Dll",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0, KEY_ALL_ACCESS,&hKeyResults);

	if (lResult == ERROR_SUCCESS)
	{
		DWORD Type, Bytes = 100;
		lResult = RegQueryValueEx(hKeyResults,"RSP Dll",0,&Type,(LPBYTE)(RspDLL),&Bytes);
		if (lResult != ERROR_SUCCESS)
			sprintf(RspDLL,"%s",DefaultRSPDll);
		Bytes = 100;
		lResult = RegQueryValueEx(hKeyResults,"Graphics Dll",0,&Type,(LPBYTE)(GfxDLL),&Bytes);
		if (lResult != ERROR_SUCCESS)
			sprintf(GfxDLL,"%s",DefaultGFXDll);
		Bytes = 100;
		lResult = RegQueryValueEx(hKeyResults,"Audio Dll",0,&Type,(LPBYTE)(AudioDLL),&Bytes);
		if (lResult != ERROR_SUCCESS)
			sprintf(AudioDLL,"%s",DefaultAudioDll);
		Bytes = 100;
		lResult = RegQueryValueEx(hKeyResults,"Controller Dll",0,&Type,(LPBYTE)(ControllerDLL),&Bytes);
		if (lResult != ERROR_SUCCESS)
			sprintf(ControllerDLL,"%s",DefaultControllerDll);
	}
	else
	{
		sprintf(RspDLL,"%s",DefaultRSPDll);
		sprintf(GfxDLL,"%s",DefaultGFXDll);
		sprintf(AudioDLL,"%s",DefaultAudioDll);
		sprintf(ControllerDLL,"%s",DefaultControllerDll);
	}
}

BOOL LoadRSPDll(char * RspDll) {
	PLUGIN_INFO PluginInfo;
	char DllName[300];

	GetPluginDir(DllName);
	strcat(DllName,RspDll);

	RspDebug.UseBPoints = FALSE;

	hRspDll = LoadLibrary(DllName);
	if (hRspDll == NULL) {  return FALSE; }

	GetDllInfo = (void (__cdecl *)(PLUGIN_INFO *))GetProcAddress( hRspDll, "GetDllInfo" );
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }
	RSPVersion = PluginInfo.Version;
	if (RSPVersion == 1) { RSPVersion = 0x0100; }

	DoRspCycles = (DWORD (__cdecl *)(DWORD))GetProcAddress( hRspDll, "DoRspCycles" );
	if (DoRspCycles == NULL) { return FALSE; }
	InitiateRSP_1_0 = NULL;
	InitiateRSP_1_1 = NULL;
	if (RSPVersion == 0x100) {
		InitiateRSP_1_0 = (void (__cdecl *)(RSP_INFO_1_0,DWORD *))GetProcAddress( hRspDll, "InitiateRSP" );
		if (InitiateRSP_1_0 == NULL) { return FALSE; }
	}
	if (RSPVersion == 0x101) {
		InitiateRSP_1_1 = (void (__cdecl *)(RSP_INFO_1_1,DWORD *))GetProcAddress( hRspDll, "InitiateRSP" );
		if (InitiateRSP_1_1 == NULL) { return FALSE; }
	}
	RSPRomClosed = (void (__cdecl *)(void))GetProcAddress( hRspDll, "RomClosed" );
	if (RSPRomClosed == NULL) { return FALSE; }
	RSPCloseDLL = (void (__cdecl *)(void))GetProcAddress( hRspDll, "CloseDLL" );
	if (RSPCloseDLL == NULL) { return FALSE; }
	GetRspDebugInfo = (void (__cdecl *)(RSPDEBUG_INFO *))GetProcAddress( hRspDll, "GetRspDebugInfo" );
	InitiateRSPDebugger = (void (__cdecl *)(DEBUG_INFO))GetProcAddress( hRspDll, "InitiateRSPDebugger" );
	RSPDllConfig = (void (__cdecl *)(HWND))GetProcAddress( hRspDll, "DllConfig" );
	return TRUE;
}

void SetupPlugins (HWND hWnd) {
	ShutdownPlugins();
	GetCurrentDlls();

	PluginsInitilized = TRUE;

	if (!LoadGFXDll(GfxDLL)) {
		CloseGfxPlugin();
		DisplayError(GS(MSG_FAIL_INIT_GFX));
		PluginsInitilized = FALSE;
	} else { 
		GFX_INFO GfxInfo;

		GfxInfo.MemoryBswaped = TRUE;
		GfxInfo.CheckInterrupts = CheckInterrupts;
		GfxInfo.hStatusBar = hStatusWnd;
		GfxInfo.hWnd = hWnd;
		GfxInfo.HEADER = (BYTE*)RomHeader;
		GfxInfo.RDRAM = N64MEM;
		GfxInfo.DMEM = DMEM;
		GfxInfo.IMEM = IMEM;
		GfxInfo.MI__INTR_REG = &MI_INTR_REG;	
		GfxInfo.DPC__START_REG = &DPC_START_REG;
		GfxInfo.DPC__END_REG = &DPC_END_REG;
		GfxInfo.DPC__CURRENT_REG = &DPC_CURRENT_REG;
		GfxInfo.DPC__STATUS_REG = &DPC_STATUS_REG;
		GfxInfo.DPC__CLOCK_REG = &DPC_CLOCK_REG;
		GfxInfo.DPC__BUFBUSY_REG = &DPC_BUFBUSY_REG;
		GfxInfo.DPC__PIPEBUSY_REG = &DPC_PIPEBUSY_REG;
		GfxInfo.DPC__TMEM_REG = &DPC_TMEM_REG;
		GfxInfo.VI__STATUS_REG = &VI_STATUS_REG;
		GfxInfo.VI__ORIGIN_REG = &VI_ORIGIN_REG;
		GfxInfo.VI__WIDTH_REG = &VI_WIDTH_REG;
		GfxInfo.VI__INTR_REG = &VI_INTR_REG;
		GfxInfo.VI__V_CURRENT_LINE_REG = &VI_CURRENT_REG;
		GfxInfo.VI__TIMING_REG = &VI_TIMING_REG;
		GfxInfo.VI__V_SYNC_REG = &VI_V_SYNC_REG;
		GfxInfo.VI__H_SYNC_REG = &VI_H_SYNC_REG;
		GfxInfo.VI__LEAP_REG = &VI_LEAP_REG;
		GfxInfo.VI__H_START_REG = &VI_H_START_REG;
		GfxInfo.VI__V_START_REG = &VI_V_START_REG;
		GfxInfo.VI__V_BURST_REG = &VI_V_BURST_REG;
		GfxInfo.VI__X_SCALE_REG = &VI_X_SCALE_REG;
		GfxInfo.VI__Y_SCALE_REG = &VI_Y_SCALE_REG;
		
		if (!InitiateGFX(GfxInfo) ) {
			CloseGfxPlugin();
			DisplayError(GS(MSG_FAIL_INIT_GFX));
			PluginsInitilized = FALSE;
		}
	}

	if (!LoadAudioDll(AudioDLL) ) {
		CloseAudioPlugin();
		DisplayError(GS(MSG_FAIL_INIT_AUDIO));
		PluginsInitilized = FALSE;
	} else {
		AUDIO_INFO AudioInfo;
		
		AudioInfo.hwnd = hWnd;
		AudioInfo.hinst = hInst;
		AudioInfo.MemoryBswaped = TRUE;
		AudioInfo.HEADER = (BYTE*)RomHeader;
		AudioInfo.RDRAM = N64MEM;
		AudioInfo.DMEM = DMEM;
		AudioInfo.IMEM = IMEM;
		AudioInfo.MI__INTR_REG = &AudioIntrReg;	
		AudioInfo.AI__DRAM_ADDR_REG = &AI_DRAM_ADDR_REG;	
		AudioInfo.AI__LEN_REG = &AI_LEN_REG;	
		AudioInfo.AI__CONTROL_REG = &AI_CONTROL_REG;	
		AudioInfo.AI__STATUS_REG = &AI_STATUS_REG;	
		AudioInfo.AI__DACRATE_REG = &AI_DACRATE_REG;	
		AudioInfo.AI__BITRATE_REG = &AI_BITRATE_REG;	
		AudioInfo.CheckInterrupts = AiCheckInterrupts;
		if (!InitiateAudio(AudioInfo)) {
			CloseAudioPlugin();
			DisplayError(GS(MSG_FAIL_INIT_AUDIO));
			PluginsInitilized = FALSE;
		}
		DWORD ThreadID;
		hAudioThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AudioThread, (LPVOID)NULL,0, &ThreadID);
	}

	if (!LoadRSPDll(RspDLL)) {  //Implement this into the actual project64 source code so its not outsource
		DisplayError(GS(MSG_FAIL_INIT_RSP));
		PluginsInitilized = FALSE;
	} else { 
		RSP_INFO_1_0 RspInfo10;
		RSP_INFO_1_1 RspInfo11;

		RspInfo10.CheckInterrupts = CheckInterrupts;
		RspInfo11.CheckInterrupts = CheckInterrupts;
		RspInfo10.ProcessDlist = ProcessDList;
		RspInfo11.ProcessDlist = ProcessDList;
		RspInfo10.ProcessAlist = ProcessAList;
		RspInfo11.ProcessAlist = ProcessAList;
		RspInfo10.ProcessRdpList = ProcessRDPList;
		RspInfo11.ProcessRdpList = ProcessRDPList;
		RspInfo11.ShowCFB = ShowCFB;

		RspInfo10.hInst = hInst;
		RspInfo11.hInst = hInst;
		RspInfo10.RDRAM = N64MEM;
		RspInfo11.RDRAM = N64MEM;
		RspInfo10.DMEM = DMEM;
		RspInfo11.DMEM = DMEM;
		RspInfo10.IMEM = IMEM;
		RspInfo11.IMEM = IMEM;
		RspInfo10.MemoryBswaped = FALSE;
		RspInfo11.MemoryBswaped = FALSE;

		RspInfo10.MI__INTR_REG = &MI_INTR_REG;
		RspInfo11.MI__INTR_REG = &MI_INTR_REG;
			
		RspInfo10.SP__MEM_ADDR_REG = &SP_MEM_ADDR_REG;
		RspInfo11.SP__MEM_ADDR_REG = &SP_MEM_ADDR_REG;
		RspInfo10.SP__DRAM_ADDR_REG = &SP_DRAM_ADDR_REG;
		RspInfo11.SP__DRAM_ADDR_REG = &SP_DRAM_ADDR_REG;
		RspInfo10.SP__RD_LEN_REG = &SP_RD_LEN_REG;
		RspInfo11.SP__RD_LEN_REG = &SP_RD_LEN_REG;
		RspInfo10.SP__WR_LEN_REG = &SP_WR_LEN_REG;
		RspInfo11.SP__WR_LEN_REG = &SP_WR_LEN_REG;
		RspInfo10.SP__STATUS_REG = &SP_STATUS_REG;
		RspInfo11.SP__STATUS_REG = &SP_STATUS_REG;
		RspInfo10.SP__DMA_FULL_REG = &SP_DMA_FULL_REG;
		RspInfo11.SP__DMA_FULL_REG = &SP_DMA_FULL_REG;
		RspInfo10.SP__DMA_BUSY_REG = &SP_DMA_BUSY_REG;
		RspInfo11.SP__DMA_BUSY_REG = &SP_DMA_BUSY_REG;
		RspInfo10.SP__PC_REG = &SP_PC_REG;
		RspInfo11.SP__PC_REG = &SP_PC_REG;
		RspInfo10.SP__SEMAPHORE_REG = &SP_SEMAPHORE_REG;
		RspInfo11.SP__SEMAPHORE_REG = &SP_SEMAPHORE_REG;
			
		RspInfo10.DPC__START_REG = &DPC_START_REG;
		RspInfo11.DPC__START_REG = &DPC_START_REG;
		RspInfo10.DPC__END_REG = &DPC_END_REG;
		RspInfo11.DPC__END_REG = &DPC_END_REG;
		RspInfo10.DPC__CURRENT_REG = &DPC_CURRENT_REG;
		RspInfo11.DPC__CURRENT_REG = &DPC_CURRENT_REG;
		RspInfo10.DPC__STATUS_REG = &DPC_STATUS_REG;
		RspInfo11.DPC__STATUS_REG = &DPC_STATUS_REG;
		RspInfo10.DPC__CLOCK_REG = &DPC_CLOCK_REG;
		RspInfo11.DPC__CLOCK_REG = &DPC_CLOCK_REG;
		RspInfo10.DPC__BUFBUSY_REG = &DPC_BUFBUSY_REG;
		RspInfo11.DPC__BUFBUSY_REG = &DPC_BUFBUSY_REG;
		RspInfo10.DPC__PIPEBUSY_REG = &DPC_PIPEBUSY_REG;
		RspInfo11.DPC__PIPEBUSY_REG = &DPC_PIPEBUSY_REG;
		RspInfo10.DPC__TMEM_REG = &DPC_TMEM_REG;
		RspInfo11.DPC__TMEM_REG = &DPC_TMEM_REG;

		if (RSPVersion == 0x0100) { InitiateRSP_1_0(RspInfo10, &RspTaskValue); }
		if (RSPVersion == 0x0101) { InitiateRSP_1_1(RspInfo11, &RspTaskValue); }
	}
	
#ifndef EXTERNAL_RELEASE
	if (HaveDebugger) {
		DEBUG_INFO DebugInfo;

		if (GetRspDebugInfo != NULL) { GetRspDebugInfo(&RspDebug); }				
		
		DebugInfo.UpdateBreakPoints = RefreshBreakPoints;
		DebugInfo.UpdateMemory = Refresh_Memory;
		DebugInfo.UpdateR4300iRegisters = UpdateCurrentR4300iRegisterPanel;
		DebugInfo.Enter_BPoint_Window = Enter_BPoint_Window;
		DebugInfo.Enter_Memory_Window = Enter_Memory_Window;
		DebugInfo.Enter_R4300i_Commands_Window = Enter_R4300i_Commands_Window;
		DebugInfo.Enter_R4300i_Register_Window = Enter_R4300i_Register_Window;
		DebugInfo.Enter_RSP_Commands_Window = RspDebug.Enter_RSP_Commands_Window;
		if (InitiateRSPDebugger != NULL) { InitiateRSPDebugger(DebugInfo); }
	}
#endif

	if (!LoadInputDll(ControllerDLL)) { 
		CloseInputPlugin();
		DisplayError(GS(MSG_FAIL_INIT_CONTROL));
		PluginsInitilized = FALSE;
	} else {
		Controllers[0].Present = FALSE;
		Controllers[0].RawData = FALSE;
		Controllers[0].Plugin  = PLUGIN_NONE;
		
		Controllers[1].Present = FALSE;
		Controllers[1].RawData = FALSE;
		Controllers[1].Plugin  = PLUGIN_NONE;
		
		Controllers[2].Present = FALSE;
		Controllers[2].RawData = FALSE;
		Controllers[2].Plugin  = PLUGIN_NONE;
		
		Controllers[3].Present = FALSE;
		Controllers[3].RawData = FALSE;
		Controllers[3].Plugin  = PLUGIN_NONE;
	
		if (ContVersion == 0x0100) {
			InitiateControllers_1_0(hWnd,Controllers);
		}
		if (ContVersion == 0x0101) {
			CONTROL_INFO ControlInfo;
			ControlInfo.Controls = Controllers;
			ControlInfo.HEADER = (BYTE*)RomHeader;
			ControlInfo.hinst = hInst;
			ControlInfo.hMainWindow = hWnd;
			ControlInfo.MemoryBswaped = TRUE;
			InitiateControllers_1_1(ControlInfo);
		}
#ifndef EXTERNAL_RELEASE
//		Controllers[0].Plugin  = PLUGIN_RUMBLE_PAK;
#endif
	}
	if (!PluginsInitilized)
		ChangeSettings(hMainWindow);
}

void SetupPluginScreen (HWND hDlg) {
	WIN32_FIND_DATA FindData;
	PLUGIN_INFO PluginInfo;
	char SearchsStr[300], Plugin[300];
	HANDLE hFind;
	HMODULE hLib;
	int index;

#ifdef EXTERNAL_RELEASE
	ShowWindow(GetDlgItem(hDlg,IDC_RSP_NAME),SW_HIDE);
	ShowWindow(GetDlgItem(hDlg,RSP_LIST),SW_HIDE);
	ShowWindow(GetDlgItem(hDlg,RSP_ABOUT),SW_HIDE);
#endif
	SetDlgItemText(hDlg,RSP_ABOUT,GS(PLUG_ABOUT));
	SetDlgItemText(hDlg,GFX_ABOUT,GS(PLUG_ABOUT));
	SetDlgItemText(hDlg,AUDIO_ABOUT,GS(PLUG_ABOUT));
	SetDlgItemText(hDlg,CONT_ABOUT,GS(PLUG_ABOUT));

	SetDlgItemText(hDlg,IDC_RSP_NAME,GS(PLUG_RSP));
	SetDlgItemText(hDlg,IDC_GFX_NAME,GS(PLUG_GFX));
	SetDlgItemText(hDlg,IDC_AUDIO_NAME,GS(PLUG_AUDIO));
	SetDlgItemText(hDlg,IDC_CONT_NAME,GS(PLUG_CTRL));
	
	GetPluginDir(SearchsStr);
	strcat(SearchsStr,"*.dll");

	hFind = FindFirstFile(SearchsStr, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	PluginCount = 0;

	for (;;)
	{
		PluginNames[PluginCount] = (char*)malloc(strlen(FindData.cFileName) + 1);
		strcpy(PluginNames[PluginCount],FindData.cFileName);
		GetPluginDir(Plugin);
		strcat(Plugin,PluginNames[PluginCount]);
		hLib = LoadLibrary(Plugin);		
		if (hLib == NULL)
		{ 
			DisplayError("%s\n %s (%d)",GS(MSG_FAIL_LOAD_PLUGIN),Plugin, GetLastError()); 
			if (FindNextFile(hFind,&FindData) == 0)
				return;
			continue;
		}
		GetDllInfo = (void (__cdecl *)(PLUGIN_INFO *))GetProcAddress( (HMODULE)hLib, "GetDllInfo" );
		if (GetDllInfo == NULL) {
			FreeLibrary((HMODULE)hLib);
			if (FindNextFile(hFind,&FindData) == 0)
				return;
			continue; 
		}
		GetDllInfo(&PluginInfo);
		if (!ValidPluginVersion(&PluginInfo) || 
			(PluginInfo.Type != PLUGIN_TYPE_CONTROLLER && PluginInfo.MemoryBswaped == FALSE))
		{
			FreeLibrary((HMODULE)hLib);
			if (FindNextFile(hFind,&FindData) == 0)
				return;
			continue;
		}


		switch(PluginInfo.Type) 
		{
#ifndef EXTERNAL_RELEASE
		case PLUGIN_TYPE_RSP:
			index = SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_ADDSTRING,(WPARAM)0, (LPARAM)&PluginInfo.Name);		
			SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_SETITEMDATA ,(WPARAM)index, (LPARAM)PluginCount);		
			if(_stricmp(RspDLL,PluginNames[PluginCount]) == 0)
			{
				SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
				RSPDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,RSP_ABOUT),RSPDllAbout != NULL ? TRUE:FALSE);
			}
			else
			{
				FreeLibrary(hLib);
			}
			break;
#endif
		case PLUGIN_TYPE_GFX:
			index = SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_ADDSTRING,(WPARAM)0, (LPARAM)&PluginInfo.Name);		
			SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_SETITEMDATA ,(WPARAM)index, (LPARAM)PluginCount);		
			if(_stricmp(GfxDLL,PluginNames[PluginCount]) == 0)
			{
				SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
		//		GFXDllAbout = (void (__cdecl *)(HWND))GetProcAddress( (HMODULE)hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,GFX_ABOUT),GFXDllAbout != NULL ? TRUE:FALSE);
			} else {
				FreeLibrary((HMODULE)hLib);
			}
			break;
		case PLUGIN_TYPE_AUDIO:
			index = SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_ADDSTRING,(WPARAM)0, (LPARAM)&PluginInfo.Name);
			SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_SETITEMDATA ,(WPARAM)index, (LPARAM)PluginCount);		
			if(_stricmp(AudioDLL,PluginNames[PluginCount]) == 0)
			{
				SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
			//	AiDllAbout = (void (__cdecl *)(HWND))GetProcAddress( (HMODULE)hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,AUDIO_ABOUT),AiDllAbout != NULL ? TRUE:FALSE);
			}
			else
			{
				FreeLibrary((HMODULE)hLib);
			}
			break;
		case PLUGIN_TYPE_CONTROLLER:
			index = SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_ADDSTRING,(WPARAM)0, (LPARAM)&PluginInfo.Name);		
			SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_SETITEMDATA ,(WPARAM)index, (LPARAM)PluginCount);		
			if(_stricmp(ControllerDLL,PluginNames[PluginCount]) == 0)
			{
				SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
		//		ContDllAbout = (void (__cdecl *)(HWND))GetProcAddress( (HMODULE)hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,CONT_ABOUT),ContDllAbout != NULL ? TRUE:FALSE);
			}
			else
			{
				FreeLibrary((HMODULE)hLib);
			}
			break;
		}
		PluginCount += 1;
		if (FindNextFile(hFind,&FindData) == 0) { return; }
	}
}

void ShutdownPlugins (void)
{
	TerminateThread(hAudioThread,0);
	CloseGfxPlugin();
	if (RSPCloseDLL != NULL) { RSPCloseDLL(); }
	CloseAudioPlugin();
	CloseInputPlugin();
	FreeLibrary(hRspDll);
	PluginsInitilized = FALSE;
}

BOOL ValidPluginVersion ( PLUGIN_INFO * PluginInfo ) {//Remove this function?
	switch (PluginInfo->Type) {
	case PLUGIN_TYPE_RSP: 
		if (PluginInfo->Version == 0x0001) { return TRUE; }
		if (PluginInfo->Version == 0x0100) { return TRUE; }
		if (PluginInfo->Version == 0x0101) { return TRUE; }
		break;
	case PLUGIN_TYPE_GFX:
		if (PluginInfo->Version == 0x0102) { return TRUE; }
		if (PluginInfo->Version == 0x0103) { return TRUE; }
		break;
	case PLUGIN_TYPE_AUDIO:
		if (PluginInfo->Version == 0x0101) { return TRUE; }
		break;
	case PLUGIN_TYPE_CONTROLLER:
		if (PluginInfo->Version == 0x0100) { return TRUE; }
		break;
	}
	return FALSE;
}
