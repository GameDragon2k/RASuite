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
#ifndef _PLUGIN_H_
#define _PLUGIN_H_


#include "Plugin_Video.h"
#include "Plugin_Audio.h"
#include "Plugin_Input.h"

#define DefaultGFXDll				"Jabo_Direct3D8.dll"
#define DefaultRSPDll				"RSP.dll"
#define DefaultAudioDll				"Jabo_Dsound.dll"
#define DefaultControllerDll		"Jabo_DInput.dll"

#define PLUGIN_TYPE_RSP				1
#define PLUGIN_TYPE_GFX				2
#define PLUGIN_TYPE_AUDIO			3
#define PLUGIN_TYPE_CONTROLLER		4

#define SYSTEM_NTSC					0
#define SYSTEM_PAL					1
#define SYSTEM_MPAL					2

typedef struct {
	WORD Version;        /* Should be set to 1 */
	WORD Type;           /* Set to PLUGIN_TYPE_GFX */
	char Name[100];      /* Name of the DLL */

	/* If DLL supports memory these memory options then set them to TRUE or FALSE
	   if it does not support it */
	BOOL NormalMemory;   /* a normal BYTE array */ 
	BOOL MemoryBswaped;  /* a normal BYTE array where the memory has been pre
	                          bswap on a dword (32 bits) boundry */
} PLUGIN_INFO;

typedef struct {
	HINSTANCE hInst;
	BOOL MemoryBswaped;    /* If this is set to TRUE, then the memory has been pre
	                          bswap on a dword (32 bits) boundry */
	BYTE * RDRAM;
	BYTE * DMEM;
	BYTE * IMEM;

	DWORD * MI__INTR_REG;

	DWORD * SP__MEM_ADDR_REG;
	DWORD * SP__DRAM_ADDR_REG;
	DWORD * SP__RD_LEN_REG;
	DWORD * SP__WR_LEN_REG;
	DWORD * SP__STATUS_REG;
	DWORD * SP__DMA_FULL_REG;
	DWORD * SP__DMA_BUSY_REG;
	DWORD * SP__PC_REG;
	DWORD * SP__SEMAPHORE_REG;

	DWORD * DPC__START_REG;
	DWORD * DPC__END_REG;
	DWORD * DPC__CURRENT_REG;
	DWORD * DPC__STATUS_REG;
	DWORD * DPC__CLOCK_REG;
	DWORD * DPC__BUFBUSY_REG;
	DWORD * DPC__PIPEBUSY_REG;
	DWORD * DPC__TMEM_REG;

	void ( __cdecl *CheckInterrupts)( void );
	void (__cdecl *ProcessDlist)( void );
	void (__cdecl *ProcessAlist)( void );
	void (__cdecl *ProcessRdpList)( void );
} RSP_INFO_1_0;


typedef struct {
	HINSTANCE hInst;
	BOOL MemoryBswaped;    /* If this is set to TRUE, then the memory has been pre
	                          bswap on a dword (32 bits) boundry */
	BYTE * RDRAM;
	BYTE * DMEM;
	BYTE * IMEM;

	DWORD * MI__INTR_REG;

	DWORD * SP__MEM_ADDR_REG;
	DWORD * SP__DRAM_ADDR_REG;
	DWORD * SP__RD_LEN_REG;
	DWORD * SP__WR_LEN_REG;
	DWORD * SP__STATUS_REG;
	DWORD * SP__DMA_FULL_REG;
	DWORD * SP__DMA_BUSY_REG;
	DWORD * SP__PC_REG;
	DWORD * SP__SEMAPHORE_REG;

	DWORD * DPC__START_REG;
	DWORD * DPC__END_REG;
	DWORD * DPC__CURRENT_REG;
	DWORD * DPC__STATUS_REG;
	DWORD * DPC__CLOCK_REG;
	DWORD * DPC__BUFBUSY_REG;
	DWORD * DPC__PIPEBUSY_REG;
	DWORD * DPC__TMEM_REG;

	void ( __cdecl *CheckInterrupts)( void );
	void (__cdecl *ProcessDlist)( void );
	void (__cdecl *ProcessAlist)( void );
	void (__cdecl *ProcessRdpList)( void );
	void (__cdecl *ShowCFB)( void );
} RSP_INFO_1_1;

typedef struct {
	/* Menu */
	/* Items should have an ID between 5001 and 5100 */
	HMENU hRSPMenu;
	void (__cdecl *ProcessMenuItem) ( int ID );

	/* Break Points */
	BOOL UseBPoints;
	char BPPanelName[20];
	void (__cdecl *Add_BPoint)      ( void );
	void (__cdecl *CreateBPPanel)   ( HWND hDlg, RECT rcBox );
	void (__cdecl *HideBPPanel)     ( void );
	void (__cdecl *PaintBPPanel)    ( PAINTSTRUCT ps );
	void (__cdecl *ShowBPPanel)     ( void );
	void (__cdecl *RefreshBpoints)  ( HWND hList );
	void (__cdecl *RemoveBpoint)    ( HWND hList, int index );
	void (__cdecl *RemoveAllBpoint) ( void );
	
	/* RSP command Window */
	void (__cdecl *Enter_RSP_Commands_Window) ( void );
} RSPDEBUG_INFO;


typedef struct {
	void (__cdecl *UpdateBreakPoints)( void );
	void (__cdecl *UpdateMemory)( void );
	void (__cdecl *UpdateR4300iRegisters)( void );
	void (__cdecl *Enter_BPoint_Window)( void );
	void (__cdecl *Enter_R4300i_Commands_Window)( void );
	void (__cdecl *Enter_R4300i_Register_Window)( void );
	void (__cdecl *Enter_RSP_Commands_Window) ( void );
	void (__cdecl *Enter_Memory_Window)( void );
} DEBUG_INFO;


/******** All DLLs have this function **************/
extern void (__cdecl *GetDllInfo)             ( PLUGIN_INFO * PluginInfo );

/********** RSP DLL: Functions *********************/
extern void (__cdecl *GetRspDebugInfo)    ( RSPDEBUG_INFO * DebugInfo );
extern void (__cdecl *RSPCloseDLL)        ( void );
extern void (__cdecl *RSPDllAbout)        ( HWND hWnd );
extern void (__cdecl *RSPDllConfig)       ( HWND hWnd );
extern void (__cdecl *RSPRomClosed)       ( void );
extern DWORD (__cdecl *DoRspCycles)       ( DWORD );
extern void (__cdecl *InitiateRSP_1_0)    ( RSP_INFO_1_0 Rsp_Info, DWORD * Cycles);
extern void (__cdecl *InitiateRSP_1_1)    ( RSP_INFO_1_1 Rsp_Info, DWORD * Cycles);
extern void (__cdecl *InitiateRSPDebugger)( DEBUG_INFO DebugInfo);


/********** Plugin: Functions *********************/
void PluginConfiguration ( HWND hWnd );
void SetupPlugins        ( HWND hWnd );
void SetupPluginScreen   ( HWND hDlg );
void ShutdownPlugins     ( void );

/********** External Global Variables ***************/
#define MaxDlls	100
extern char RspDLL[100], * PluginNames[MaxDlls];
extern DWORD PluginCount, RspTaskValue, AudioIntrReg;
extern RSPDEBUG_INFO RspDebug;
extern WORD RSPVersion;
extern BOOL PluginsInitilized;

#endif// _PLUGIN_H_