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
#include <time.h>
#include "main.h"
#include "CPU.h"
#include "FileUtil.h"

static HANDLE hEepromFile = NULL;
BYTE EEPROM[0x800];
	
unsigned char byte2bcd(int n)
{
	n %= 100;
	return ((n / 10) << 4) | (n % 10);
}

void CloseEeprom (void) {
	if (hEepromFile) {
		CloseHandle(hEepromFile);
		hEepromFile = NULL;
	}
}

void EepromCommand ( BYTE * Command) 
{
	time_t curtime_time;
	struct tm curtime;
	if (SaveUsing == Auto)
		SaveUsing = Eeprom_4K;

	switch (Command[2])
	{
		case 0: // check eeprom size
			if (SaveUsing != Eeprom_4K &&  SaveUsing != Eeprom_16K)
			{
				Command[1] |= 0x80;
				break;
			}
			if (Command[1] != 3) 
			{ 
				Command[1] |= 0x40; 
				if ((Command[1] & 3) > 0)
					Command[3] = 0x00;
				if ((Command[1] & 3) > 1)
					Command[4] = (SaveUsing != Eeprom_16K) ? 0x80 : 0xC0;
				if ((Command[1] & 3) > 2)
					Command[5] = 0x00;
			} 
			else
			{
				Command[3] = 0x00;
				Command[4] = (SaveUsing != Eeprom_16K) ? 0x80 : 0xC0;
				Command[5] = 0x00;
			}
			break;
		case 4: // Read from Eeprom
			if (Command[0] != 2 || Command[1] != 8)
				DebugError("What am I meant to do with this Eeprom Command");
			ReadFromEeprom(&Command[4],Command[3]);
			break;
		case 5: //Write to Eeprom
			if (Command[0] != 10 || Command[1] != 1)
				DebugError("What am I meant to do with this Eeprom Command");
			WriteToEeprom(&Command[4],Command[3]);
			break;

		case 6: //RTC Support, Credit Mupen64 Source
			//RTC Status Query
			Command[3]  = 0x00;
			Command[4]  = 0x10;
			Command[12] = 0x00;
			break;
		case 7:
			//Read RTC Block
			switch(Command[3]) //Block number
			{
				case 0:
					Command[4] = 0x00;
					Command[5] = 0x02;
					Command[12] = 0x00;
					break;

				case 1: //Read Block
					//Read block, Command[2]
					break;

				case 2: //Set RTC time
					time(&curtime_time);
					memcpy(&curtime, localtime(&curtime_time), sizeof(curtime)); // fd's fix
					Command[4] = byte2bcd(curtime.tm_sec);
					Command[5] = byte2bcd(curtime.tm_min);
					Command[6] = 0x80 + byte2bcd(curtime.tm_hour);
					Command[7] = byte2bcd(curtime.tm_mday);
					Command[8] = byte2bcd(curtime.tm_wday);
					Command[9] = byte2bcd(curtime.tm_mon + 1);
					Command[10] = byte2bcd(curtime.tm_year);
					Command[11] = byte2bcd(curtime.tm_year / 100);
					Command[12] = 0x00;	// status
					break;
			}
			break;
		case 8:
			//Write RTC Block
			break;
		default:
			if (ShowPifRamErrors) 
				DisplayError("Unkown EepromCommand %d",Command[2]);
			break;
	}
}


void LoadEeprom (void) {
	char File[255], Directory[255];
	DWORD dwRead;
	GetAutoSaveDir(Directory);
	sprintf(File,"%s%s.eep",Directory,RomName);
	
	hEepromFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (hEepromFile == INVALID_HANDLE_VALUE) 
	{
		DisplayError(GS(MSG_FAIL_OPEN_EEPROM));
		return;
	}
	memset(EEPROM,0,sizeof(EEPROM));
	SetFilePointer(hEepromFile,0,NULL,FILE_BEGIN);	
	ReadFile(hEepromFile,EEPROM,sizeof(EEPROM),&dwRead,NULL);
}

void ReadFromEeprom(BYTE * Buffer, int line) {
	int i;
	
	if (hEepromFile == NULL)
		LoadEeprom();

	for(i=0;i<8;i++) 
		Buffer[i]=EEPROM[line*8+i];
}

void WriteToEeprom(BYTE * Buffer, int line) {
	DWORD dwWritten;
	int i;

	if (hEepromFile == NULL)
		LoadEeprom();

	for(i=0;i<8;i++) 
		EEPROM[line*8+i]=Buffer[i];

	SetFilePointer(hEepromFile,line*8,NULL,FILE_BEGIN);	
	WriteFile( hEepromFile,Buffer,8,&dwWritten,NULL );
}