// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//-----------------------------------------------------------------------------
/// \file
/// \brief Main program, simply calls D_SRB2Main and D_SRB2Loop, the high level loop.

#include "../doomdef.h"
#include "../m_argv.h"
#include "../d_main.h"
#include "../i_system.h"
#include "../d_clisrv.h"

#ifdef __GNUC__
#include <unistd.h>
#endif

#include "time.h" // For log timestamps

#ifdef HAVE_TTF
#include "i_ttf.h"
#endif

#ifdef LOGMESSAGES
FILE *logstream = NULL;
char  logfilename[1024];
#endif

#ifndef DOXYGEN
#ifndef O_TEXT
#define O_TEXT 0
#endif

#ifndef O_SEQUENTIAL
#define O_SEQUENTIAL 0
#endif
#endif

#if defined (_WIN32)
#include "../win32/win_dbg.h"
typedef BOOL (WINAPI *p_IsDebuggerPresent)(VOID);
#endif

/**	\brief	The main function

	\param	argc	number of arg
	\param	*argv	string table

	\return	int
*/
#if defined (__GNUC__) && (__GNUC__ >= 4)
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#endif

int main(int argc, char **argv)
{
	const char *logdir = NULL;
	myargc = argc;
	myargv = argv; /// \todo pull out path to exe from this string

	dedicated = true;

	// disable text input right off the bat, since we don't need it at the start.
	I_SetTextInputMode(false);

#ifdef LOGMESSAGES
	if (!M_CheckParm("-nolog"))
	{
		time_t my_time;
		struct tm * timeinfo;
		char buf[26];
		logdir = D_Home();
		my_time = time(NULL);
		timeinfo = localtime(&my_time);
		strftime(buf, 26, "%Y-%m-%d %H-%M-%S", timeinfo);
		strcpy(logfilename, va("log-%s.txt", buf));
#ifdef DEFAULTDIR
		if (logdir)
		{
			// Create dirs here because D_SRB2Main() is too late.
			I_mkdir(va("%s%s"DEFAULTDIR, logdir, PATHSEP), 0755);
			I_mkdir(va("%s%s"DEFAULTDIR"%slogs",logdir, PATHSEP, PATHSEP), 0755);
			strcpy(logfilename, va("%s%s"DEFAULTDIR"%slogs%s%s",logdir, PATHSEP, PATHSEP, PATHSEP, logfilename));
		}
		else
#endif
		{
			I_mkdir("."PATHSEP"logs"PATHSEP, 0755);
			strcpy(logfilename, va("."PATHSEP"logs"PATHSEP"%s", logfilename));
		}
		logstream = fopen(logfilename, "wt");
#endif
	}
	//I_OutputMsg("I_StartupSystem() ...\n");
	I_StartupSystem();
#if defined (_WIN32)
	LoadLibraryA("exchndl.dll");
#ifndef __MINGW32__
	prevExceptionFilter = SetUnhandledExceptionFilter(RecordExceptionInfo);
#endif
#endif
	// startup SRB2
	CONS_Printf("Setting up SRB2...\n");
	D_SRB2Main();
#ifdef LOGMESSAGES
	if (!M_CheckParm("-nolog"))
		CONS_Printf("Logfile: %s\n", logfilename);
#endif
	CONS_Printf("Entering main game loop...\n");
	// never return
	D_SRB2Loop();

#ifdef BUGTRAP
	// This is safe even if BT didn't start.
	ShutdownBugTrap();
#endif

	// return to OS
	return 0;
}
