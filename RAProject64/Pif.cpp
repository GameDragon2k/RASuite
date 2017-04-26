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
#include "cpu.h"
#include "plugin.h"
#include "Debugger.h"


void ProcessControllerCommand ( int Control, BYTE * Command);
void ReadControllerCommand (int Control, BYTE * Command);

BYTE PifRom[0x7C0], *PIF_Ram;
#define CHL_LEN 0x20
void n64_cic_nus_6105(char chl[], char rsp[], int len)
{
	static char lut0[0x10] = {
		0x4, 0x7, 0xA, 0x7, 0xE, 0x5, 0xE, 0x1, 
		0xC, 0xF, 0x8, 0xF, 0x6, 0x3, 0x6, 0x9
	};
	static char lut1[0x10] = {
		0x4, 0x1, 0xA, 0x7, 0xE, 0x5, 0xE, 0x1, 
		0xC, 0x9, 0x8, 0x5, 0x6, 0x3, 0xC, 0x9
	};
	char key, *lut;
	int i, sgn, mag, mod;

	for (key = 0xB, lut = lut0, i = 0; i < len; i++) {
		rsp[i] = (key + 5 * chl[i]) & 0xF;
		key = lut[rsp[i]];
		sgn = (rsp[i] >> 3) & 0x1;
		mag = ((sgn == 1) ? ~rsp[i] : rsp[i]) & 0x7;
		mod = (mag % 3 == 1) ? sgn : 1 - sgn;
		if (lut == lut1 && (rsp[i] == 0x1 || rsp[i] == 0x9))
			mod = 1;
		if (lut == lut1 && (rsp[i] == 0xB || rsp[i] == 0xE))
			mod = 0;
		lut = (mod == 1) ? lut1 : lut0;
	}
}

int GetCicChipID (BYTE * RomData)
{
	_int64 CRC = 0;
	int count;

	for (count = 0x40; count < 0x1000; count += 4)
		CRC += *(DWORD *)(RomData+count);

	switch (CRC) 
	{
		case 0x000000D0027FDF31: return 1;
		case 0x000000CFFB631223: return 1;
		case 0x000000D057C85244: return 2;
		case 0x000000D6497E414B: return 3;
		case 0x0000011A49F60E96: return 5;
		case 0x000000D6D5BE5580: return 6;
		default:
			return -1;
	}
}

void LogControllerPakData (char * Description)
{
#ifndef EXTERNAL_RELEASE
	int count, count2;
	char HexData[100], AsciiData[100], Addon[20];
	LogMessage("\t%s:",Description);			
	LogMessage("\t------------------------------");
	for (count = 0; count < 16; count ++ )
	{
		if ((count % 4) == 0) { 
			sprintf(HexData,"\0"); 
			sprintf(AsciiData,"\0"); 
		}
 		sprintf(Addon,"%02X %02X %02X %02X", 
			PIF_Ram[(count << 2) + 0], PIF_Ram[(count << 2) + 1], 
			PIF_Ram[(count << 2) + 2], PIF_Ram[(count << 2) + 3] );
		strcat(HexData,Addon);
		if (((count + 1) % 4) != 0)
		{
			sprintf(Addon,"-");
			strcat(HexData,Addon);
		} 
		
		Addon[0] = 0;
		for (count2 = 0; count2 < 4; count2++)
		{
			if (PIF_Ram[(count << 2) + count2] < 30)
				strcat(Addon,".");
			else
				sprintf(Addon,"%s%c",Addon,PIF_Ram[(count << 2) + count2]);
		}
		strcat(AsciiData,Addon);
		
		if (((count + 1) % 4) == 0)
			LogMessage("\t%s %s",HexData, AsciiData);
	}
	LogMessage("");
#endif
}

void PifRamRead (void) 
{
	int Channel = 0, CurPos = 0;
	while( CurPos < 0x40 ) 
	{
		switch(PIF_Ram[CurPos]) 
		{
			case 0x00:
				Channel += 1;
				if (Channel > 6)
					CurPos = 0x40;
				break;
			case 0xFE:
				CurPos = 0x40; 
				break;
			case 0xFF:
			case 0xB4:
			case 0x56:
			case 0xB8:
				break; /* ??? */
			default:
				if ((PIF_Ram[CurPos] & 0xC0) == 0)
				{
					if (Channel < 4)
					{
						if (Controllers[Channel].Present && Controllers[Channel].RawData) 
						{
							ReadController(Channel,&PIF_Ram[CurPos]);
						} 
						else 
						{
							ReadControllerCommand(Channel,&PIF_Ram[CurPos]);
						}
					} 
					CurPos += PIF_Ram[CurPos] + (PIF_Ram[CurPos + 1] & 0x3F) + 1;
					Channel += 1;
				} 
				else 
				{
					if (ShowPifRamErrors) 
						DisplayError("Unknown Command in PifRamRead(%X)",PIF_Ram[CurPos]);
					CurPos = 0x40;
				}
				break;
		}
		CurPos++;
	};

	ReadController(-1,NULL);
}

void PifRamWrite (void)
{
	int Channel = 0, CurPos = 0;
	char Challenge[30], Response[30];
	Channel = 0;

	if( PIF_Ram[0x3F] > 0x1)
	{ 
		switch (PIF_Ram[0x3F]) 
		{
		case 0x02:
			// format the 'challenge' message into 30 nibbles for X-Scale's CIC code
			for (int i = 0; i < 15; i++)
			{
				Challenge[i*2] =   (PIF_Ram[48+i] >> 4) & 0x0f;
				Challenge[i*2+1] =  PIF_Ram[48+i]       & 0x0f;
			}
			// calculate the proper response for the given challenge (X-Scale's algorithm)
			n64_cic_nus_6105(Challenge, Response, CHL_LEN - 2);
			// re-format the 'response' into a byte stream
			for (int i = 0; i < 15; i++)
			{
				PIF_Ram[48+i] = (Response[i*2] << 4) + Response[i*2+1];
			}
			// the last byte (2 nibbles) is always 0
			PIF_Ram[63] = 0;
			break;
		case 0x08: 
			PIF_Ram[0x3F] = 0; 
			MI_INTR_REG |= MI_INTR_SI;
			SI_STATUS_REG |= SI_STATUS_INTERRUPT;
			CheckInterrupts();
			break;
		case 0x10:
			memset(PifRom,0,0x7C0);
			break;
		case 0x30:
			PIF_Ram[0x3F] = 0x80;		
			break;
		case 0xC0:
			memset(PIF_Ram,0,0x40);
			break;
		default:
			if (ShowPifRamErrors)
				DisplayError("Unkown PifRam control: %d",PIF_Ram[0x3F]);
		}
		return;
	}

	while (CurPos < 0x40)
	{
		switch(PIF_Ram[CurPos])
		{
			case 0x00: 
				Channel ++; 
				if (Channel > 6)
					CurPos = 0x40;
				break;
			case 0xFE: 
				CurPos = 0x40; 
				break;
			case 0xFF: 
			case 0xB4: 
			case 0x56: 
			case 0xB8: 
				break; /* ??? */
			default:
				if (!(PIF_Ram[CurPos] & 0xC0)) 
				{
					if (Channel < 4) 
					{
						if (Controllers[Channel].Present && Controllers[Channel].RawData)
								ControllerCommand(Channel,&PIF_Ram[CurPos]);
						else 
							ProcessControllerCommand(Channel,&PIF_Ram[CurPos]);
					}
					else if (Channel == 4)
						EepromCommand(&PIF_Ram[CurPos]);
					else 
						DebugError("Command on channel 5?");
					CurPos += PIF_Ram[CurPos] + (PIF_Ram[CurPos + 1] & 0x3F) + 1;
					Channel += 1;
				} 
				else 
				{
					if (ShowPifRamErrors)
						DisplayError("Unknown Command in PifRamWrite(%X)",PIF_Ram[CurPos]);
					CurPos = 0x40;
				}
				break;
		}
		CurPos++;
	}
	PIF_Ram[0x3F] = 0;
	ControllerCommand(-1,NULL);
}

void ProcessControllerCommand ( int Control, BYTE * Command) 
{
	switch (Command[2]) 
	{
	case 0x00: // check
	case 0xFF: // reset & check ?
		if ((Command[1] & 0x80) != 0)
			break;
		if (Command[0] != 1 || Command[1] != 3)
			DebugError("What am I meant to do with this Controller Command");

		if (Controllers[Control].Present) 
		{
			Command[3] = 0x05;
			Command[4] = 0x00;
			switch ( Controllers[Control].Plugin) 
			{
				case PLUGIN_RUMBLE_PAK: 
					Command[5] = 1; 
					break;
				case PLUGIN_MEMPAK: 
					Command[5] = 1; break;
				case PLUGIN_RAW: 
					Command[5] = 1; 
					break;
				default: 
					Command[5] = 0; 
					break;
			}
		}
		else
		{
			Command[1] |= 0x80;
		}
		break;

	case 0x01: // read controller
		if (Command[0] != 1 || Command[1] != 4)
			DebugError("What am I meant to do with this Controller Command");
		if (!Controllers[Control].Present)
			Command[1] |= 0x80;
		break;
	case 0x02: //read from controller pack
		if (LogOptions.LogControllerPak) 
			LogControllerPakData("Read: Before Gettting Results");
		if (Command[0] != 3 || Command[1] != 33)
			DebugError("What am I meant to do with this Controller Command");
		if (Controllers[Control].Present)
		{
			DWORD address = ((Command[3] << 8) | Command[4]);
			switch (Controllers[Control].Plugin) 
			{
				case PLUGIN_RUMBLE_PAK:
					memset(&Command[5], (address >= 0x8000 && address < 0x9000) ? 0x80 : 0x00, 0x20);
					Command[0x25] = Mempacks_CalulateCrc(&Command[5]);
					break;
				case PLUGIN_MEMPAK: 
					ReadFromMempak(Control, address, &Command[5]); 
					break;
				case PLUGIN_RAW: 
					ControllerCommand(Control, Command);
					break;
				default:
					memset(&Command[5], 0, 0x20);
					Command[0x25] = 0;
					break;
			}
		} 
		else
		{
			Command[1] |= 0x80;
		}
#ifndef EXTERNAL_RELEASE
		if (LogOptions.LogControllerPak) 
			LogControllerPakData("Read: After Gettting Results");
#endif
		break;
	case 0x03: //write controller pak
		if (LogOptions.LogControllerPak) 
			LogControllerPakData("Write: Before Processing");
		if (Command[0] != 35 || Command[1] != 1) 
			DebugError("What am I meant to do with this Controller Command");
		
		if (Controllers[Control].Present)
		{
			DWORD address = ((Command[3] << 8) | Command[4]);
			switch (Controllers[Control].Plugin) 
			{
				case PLUGIN_MEMPAK: 
					WriteToMempak(Control, address, &Command[5]); 
					break;
				case PLUGIN_RAW: 
					ControllerCommand(Control, Command);
					break;
				case PLUGIN_RUMBLE_PAK: 
					RumbleCommand(Control, *(BOOL *)(&Command[5]));
					break;
				default:
					Command[0x25] = Mempacks_CalulateCrc(&Command[5]);
					break;
			}
		} else {
			Command[1] |= 0x80;
		}
#ifndef EXTERNAL_RELEASE
		if (LogOptions.LogControllerPak)
			LogControllerPakData("Write: After Processing");
#endif
		break;
	default:
		if (ShowPifRamErrors) 
			DisplayError("Unknown ControllerCommand %d",Command[2]);
	}
}

void ReadControllerCommand (int Control, BYTE * Command)
{
	switch (Command[2])
	{
		case 0x01: // read controller
			if (Controllers[Control].Present) 
			{
				if (Command[0] != 1 || Command[1] != 4) 
					DebugError("What am I meant to do with this Controller Command");
				BUTTONS Keys;
				
				GetKeys(Control,&Keys);
				*(DWORD *)&Command[3] = Keys.Value;
			}
			break;

		case 0x02: //read from controller pack
		case 0x03: //write controller pak
			if (Controllers[Control].Present && Controllers[Control].Plugin == PLUGIN_RAW) 
				ReadController(Control, Command);
			break;
	}
}

int GetTvSystem(int country) 
{
	switch (country) 
	{
		case 0x44: //Germany
		case 0x46: //french
		case 0x49: //Italian
		case 0x50: //Europe
		case 0x53: //Spanish
		case 0x55: //Australia
		case 0x58: // X (PAL)
		case 0x59: // X (PAL)
			return SYSTEM_PAL;
			break;
		case 0: //None
		case 0x37: // 7 (Beta)
		case 0x41: // ????
		case 0x45: //USA
		case 0x4A: //Japan
			return SYSTEM_NTSC;
			break;
		default:
			return -1;
			DisplayError(GS(MSG_UNKNOWN_COUNTRY));
			break;
	}
}

int LoadPifRom() 
{
	char PifRomName[255];
	HANDLE hPifFile;
	DWORD dwRead;

	strcpy(PifRomName, main_directory);

	if(CountryTvSystem == SYSTEM_PAL)
		strcat(PifRomName,"Pifrom\\pifpal.raw");
	else if (CountryTvSystem == SYSTEM_NTSC)
		strcat(PifRomName,"Pifrom\\pifntsc.raw");
	else
		DisplayError(GS(MSG_UNKNOWN_COUNTRY));

	hPifFile = CreateFile(PifRomName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (hPifFile == INVALID_HANDLE_VALUE) 
	{
		memset(PifRom,0,0x7C0);
		return FALSE;
	}
	SetFilePointer(hPifFile,0,NULL,FILE_BEGIN);
	ReadFile(hPifFile,PifRom,0x7C0,&dwRead,NULL);	
	CloseHandle( hPifFile );
	return TRUE;
}
