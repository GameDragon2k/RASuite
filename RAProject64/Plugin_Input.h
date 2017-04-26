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
#ifndef _PLUGIN_INPUT_H_
#define _PLUGIN_INPUT_H_

/*** Conteroller plugin's ****/
#define PLUGIN_NONE					1
#define PLUGIN_MEMPAK				2
#define PLUGIN_RUMBLE_PAK			3 
#define PLUGIN_TANSFER_PAK			4 // not implemeted for non raw data
#define PLUGIN_RAW					5 // the controller plugin is passed in raw data

struct CONTROL{
	BOOL Present;
	BOOL RawData;
	int  Plugin;
};

struct CONTROL_INFO{
	HWND hMainWindow;
	HINSTANCE hinst;

	BOOL MemoryBswaped;		// If this is set to TRUE, then the memory has been pre
							//   bswap on a dword (32 bits) boundry, only effects header. 
							//	eg. the first 8 bytes are stored like this:
							//        4 3 2 1   8 7 6 5
	BYTE * HEADER;			// This is the rom header (first 40h bytes of the rom)
	CONTROL *Controls;		// A pointer to an array of 4 controllers .. eg:
							// CONTROL Controls[4];
};

union BUTTONS {
	DWORD Value;
	struct {
		unsigned R_DPAD       : 1;
		unsigned L_DPAD       : 1;
		unsigned D_DPAD       : 1;
		unsigned U_DPAD       : 1;
		unsigned START_BUTTON : 1;
		unsigned Z_TRIG       : 1;
		unsigned B_BUTTON     : 1;
		unsigned A_BUTTON     : 1;

		unsigned R_CBUTTON    : 1;
		unsigned L_CBUTTON    : 1;
		unsigned D_CBUTTON    : 1;
		unsigned U_CBUTTON    : 1;
		unsigned R_TRIG       : 1;
		unsigned L_TRIG       : 1;
		unsigned Reserved1    : 1;
		unsigned Reserved2    : 1;

		signed   Y_AXIS       : 8;

		signed   X_AXIS       : 8;
	};
} ;

extern char ControllerDLL[100];
extern WORD ContVersion;
extern CONTROL Controllers[4];

/****************
**DLL FUNCTIONS**
*****************/
void ContCloseDLL     ( void );
void ControllerCommand( int Control, BYTE * Command );
void ContDllAbout     ( HWND hParent );
void ContConfig       ( HWND hParent );
void InitiateControllers_1_0( HWND hMainWindow, CONTROL Controls[4] );
void InitiateControllers_1_1( CONTROL_INFO ControlInfo );
void GetKeys          ( int Control, BUTTONS * Keys );
void ReadController   ( int Control, BYTE * Command );
void ContRomOpen      ( void );
void ContRomClosed    ( void );
void WM_KeyDown       ( WPARAM wParam, LPARAM lParam );
void WM_KeyUp         ( WPARAM wParam, LPARAM lParam );
void RumbleCommand	 ( int Control, BOOL bRumble );

extern void (__cdecl *_ContDllAbout)     ( HWND hParent );

/***********************
**Project 64 Functions**
************************/
BOOL LoadInputDll(char * InputDll);
void CloseInputPlugin();

#endif //_PLUGIN_INPUT_H_