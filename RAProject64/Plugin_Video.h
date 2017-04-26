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
#ifndef _PLUGIN_VIDEO_H_
#define _PLUGIN_VIDEO_H_

extern char GfxDLL[100];

struct GFX_INFO {
	HWND hWnd;			/* Render window */
	HWND hStatusBar;    /* if render window does not have a status bar then this is NULL */

	BOOL MemoryBswaped;    // If this is set to TRUE, then the memory has been pre
	                       //   bswap on a dword (32 bits) boundry 
						   //	eg. the first 8 bytes are stored like this:
	                       //        4 3 2 1   8 7 6 5

	BYTE * HEADER;	// This is the rom header (first 40h bytes of the rom
					// This will be in the same memory format as the rest of the memory.
	BYTE * RDRAM;
	BYTE * DMEM;
	BYTE * IMEM;

	DWORD * MI__INTR_REG;

	DWORD * DPC__START_REG;
	DWORD * DPC__END_REG;
	DWORD * DPC__CURRENT_REG;
	DWORD * DPC__STATUS_REG;
	DWORD * DPC__CLOCK_REG;
	DWORD * DPC__BUFBUSY_REG;
	DWORD * DPC__PIPEBUSY_REG;
	DWORD * DPC__TMEM_REG;

	DWORD * VI__STATUS_REG;
	DWORD * VI__ORIGIN_REG;
	DWORD * VI__WIDTH_REG;
	DWORD * VI__INTR_REG;
	DWORD * VI__V_CURRENT_LINE_REG;
	DWORD * VI__TIMING_REG;
	DWORD * VI__V_SYNC_REG;
	DWORD * VI__H_SYNC_REG;
	DWORD * VI__LEAP_REG;
	DWORD * VI__H_START_REG;
	DWORD * VI__V_START_REG;
	DWORD * VI__V_BURST_REG;
	DWORD * VI__X_SCALE_REG;
	DWORD * VI__Y_SCALE_REG;

	void (__cdecl *CheckInterrupts)( void );
};

//DLL Functions
void CaptureScreen( char * );
void ChangeWindow( void );
void GFXCloseDLL( void );
void GFXDllAbout( HWND hParent );
void GFXDllConfig( HWND hParent );
void GfxRomClosed( void );
void GfxRomOpen( void );
void DrawScreen( void );
void FrameBufferRead( DWORD addr );
void FrameBufferWrite( DWORD addr, DWORD Bytes );
BOOL InitiateGFX( GFX_INFO Gfx_Info );
void MoveScreen( int xpos, int ypos );
void ProcessDList( void );
void ProcessRDPList( void );
void ShowCFB( void );
void UpdateScreen( void );
void ViStatusChanged( void );
void ViWidthChanged( void );

extern void (__cdecl *_GFXDllAbout)( HWND hParent );

//Project64 Functions
BOOL LoadGFXDll(char * RspDll);
void CloseGfxPlugin();

#endif //_PLUGIN_VIDEO_H_