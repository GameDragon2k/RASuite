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
#include <string.h>
#include "FileUtil.h"
#include <sys/stat.h>

#ifndef S_ISDIR
#define S_ISDIR(m)  (((m)&S_IFMT) == S_IFDIR)
#endif 
namespace File
{
	//Checks if a file exists, returns true if it does
	bool Exists(const char* filename)
	{
		struct stat file_info;
	
		int result = stat(filename, &file_info);

		return (result == 0);
	}

	//Checks if a file is a directry, Returns true if filename is a directory
	bool IsDirectory(const char* filename)
	{
		struct stat file_info;

		int result = stat(filename, &file_info);

		if(result < 0) //Stat failed
			return false;

		//Return wether the name supplid is a directory or not
		return S_ISDIR(file_info.st_mode);
	}

	//Creates a directory by path name
	bool CreateDir(const char* path)
	{
		//Create the directory
		if(CreateDirectory(path, NULL))
			return true;

		//Grab the last error
		DWORD error = GetLastError();
		//If directory already exists return true
		if (error == ERROR_ALREADY_EXISTS)
			return true;
		return false;
	}
}

//Move these elsewhere
void GetAutoSaveDir( char * Directory ) 
{
	char Dir[255], Group[200];
	long lResult;
	HKEY hKeyResults = 0;

	strcpy(Directory, main_directory);
	strcat(Directory, "Save\\");

	sprintf(Group,"Software\\N64 Emulation\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS, &hKeyResults);
	if (lResult == ERROR_SUCCESS) 
	{
		DWORD Type, Value, Bytes;

		Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"Use Default Auto Save Dir",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult == ERROR_SUCCESS && Value == FALSE) 
		{					
			Bytes = sizeof(Dir);
			lResult = RegQueryValueEx(hKeyResults,"Auto Save Directory",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult == ERROR_SUCCESS) 
				strcpy(Directory,Dir);
		}
	}
	RegCloseKey(hKeyResults);

	if(!File::IsDirectory(Directory))
		File::CreateDir(Directory);

}

//Move these elsewhere
void GetInstantSaveDir( char * Directory ) 
{
	char Dir[255], Group[200];
	long lResult;
	HKEY hKeyResults = 0;

	strcpy(Directory, main_directory);
	strcat(Directory, "Save\\");

	sprintf(Group,"Software\\N64 Emulation\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS, &hKeyResults);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD Type, Value, Bytes;

		Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"Use Default Instant Save Dir",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult == ERROR_SUCCESS && Value == FALSE) 
		{					
			Bytes = sizeof(Dir);
			lResult = RegQueryValueEx(hKeyResults,"Instant Save Directory",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult == ERROR_SUCCESS)
				strcpy(Directory,Dir);
		}
	}
	RegCloseKey(hKeyResults);	

	if(!File::IsDirectory(Directory))
		File::CreateDir(Directory);
}

//Move these elsewhere
void GetPluginDir( char * Directory )
{
	char Dir[255], Group[200];
	long lResult;
	HKEY hKeyResults = 0;

	strcpy(Directory, main_directory);
	strcat(Directory, "Plugin\\");

	sprintf(Group,"Software\\N64 Emulation\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD Type, Value, Bytes;

		Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"Use Default Plugin Dir",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult == ERROR_SUCCESS && Value == FALSE)
		{					
			Bytes = sizeof(Dir);
			lResult = RegQueryValueEx(hKeyResults,"Plugin Directory",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult == ERROR_SUCCESS)
				strcpy(Directory,Dir);
		}
	}
	RegCloseKey(hKeyResults);	

	if(!File::IsDirectory(Directory))
		File::CreateDir(Directory);
}

//Move these elsewhere
void GetSnapShotDir( char * Directory )
{
	char Dir[255], Group[200];
	long lResult;
	HKEY hKeyResults = 0;

	strcpy(Directory, main_directory);
	strcat(Directory, "Screenshots\\");

	sprintf(Group,"Software\\N64 Emulation\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD Type, Value, Bytes;

		Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"Use Default Snap Shot Dir",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult == ERROR_SUCCESS && Value == FALSE)
		{					
			Bytes = sizeof(Dir);
			lResult = RegQueryValueEx(hKeyResults,"Snap Shot Directory",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult == ERROR_SUCCESS)
				strcpy(Directory,Dir);
		}
	}
	RegCloseKey(hKeyResults);	

	if(!File::IsDirectory(Directory))
		File::CreateDir(Directory);
}