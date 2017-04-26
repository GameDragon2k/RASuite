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
#ifndef _PLUGIN_AUDIO_H
#define _PLUGIN_AUDIO_H
//Why the heck does everything need to be externed :S
extern char AudioDLL[100];
extern HINSTANCE hAudioDll;
extern HANDLE hAudioThread;

struct AUDIO_INFO{
	HWND hwnd;
	HINSTANCE hinst;

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

	DWORD * AI__DRAM_ADDR_REG;
	DWORD * AI__LEN_REG;
	DWORD * AI__CONTROL_REG;
	DWORD * AI__STATUS_REG;
	DWORD * AI__DACRATE_REG;
	DWORD * AI__BITRATE_REG;

	void (__cdecl *CheckInterrupts)( void );
};


//DLL function
void AiCloseDll			(void);
void AiDacrateChanged	(int SystemType);
void AiLenChanged		( void );
void AiDllAbout			( HWND hParent );
void AiDllConfig		( HWND hParent );
void AiDllTest			( HWND hParent );
DWORD  AiReadLength		( void );
void AiRomClosed		( void );
void  AiUpdate			( BOOL Wait );
BOOL InitiateAudio		( AUDIO_INFO Audio_Info );
void ProcessAList		( void );

extern void (__cdecl *_AiDllAbout) ( HWND hParent );

//Other functions
bool LoadAudioDll(char * AudioDll);
void CloseAudioPlugin();
void AudioThread(void);

#endif// _PLUGIN_AUDIO_H_