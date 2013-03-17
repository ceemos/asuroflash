/***************************************************************************
                                   Asuro.h 
                             -------------------
    begin                : Die Aug 12 10:16:57 CEST 2003
    copyright            : (C) 2003-2004 DLR RM by Jan Grewe
    email                : jan.grewe@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 +   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
	Ver      date       Author        comment
	--------------------------------------------------------------------------
	1.0   12.08.2003    Jan Grewe     build
  1.1   27.08.2004    Jan Grewe     find possible interfaces
 ***************************************************************************/
#ifndef ASURO_H
#define ASURO_H

#if defined WIN32 || defined _WIN32 || defined _WIN32_
#define WINDOWS
#endif

#if defined LINUX || defined _LINUX || defined _LINUX_
#define LINUX
#endif

#define START_PAGE 0x0000
#define END_PAGE   0x2000

static const char*  ASUROAbout = "ASURO Flash Tool\n\
Version 2\n\
Author: Jan Grewe (DLR), Marcel Schneider\n";
static const char*  LICENSE = "based on: ASURO Flash Copyright (c)2003-2004 DLR RM\n\
asuroflash comes with\n\
ABSOLUTELY NO WARRANTY \n\
This program is free software\n\
you can redistribute it and/or modify\n\
it under the terms of the\n\
GNU General Public License\n\
as published by\n\
the Free Software Foundation\n\
either version 2 of the License\n\
or any later version\n";

#ifdef WINDOWS
#include "stdafx.h"
#endif
#ifdef LINUX
#include <unistd.h>
#endif
#include <iostream>

#include "hexfile.h"

#define TMPFILE "/tmp/asuro-current.hex"

class CAsuro  
{
public:
	void Programm (void);
	bool LoadIni(char*);
        void SaveIni(char*);
	CAsuro();
        
        void ViewUpdate(void) {};
        
        #define ErrorText MessageText
        #define SuccessText WarningText
        #define Status MessageText
        
        void MessageText(std::string text)
        {
                std::cout << std::endl << text << std::flush;
        }
        
        void WarningText(std::string text)
        {
                std::cout << text << std::flush;
        }
        
        void Progress(unsigned int progress)
        {
                std::cout << "." << std::flush;
        }

        void TimeWait(unsigned int time) // msec
        {
        #ifdef WINDOWS
                Sleep(time);
        #elif defined LINUX
                usleep(time * 1000);
        #endif
        }

	bool SendPage(unsigned int number);
        void FinalPage(void);
	bool BuildRAM(void);
	bool Connect(void);
	bool Init(void);
	unsigned int CRC16 (unsigned int crc, unsigned char data);

	bool m_ASUROCancel;
	char m_ASUROCOMPort[255];
	char m_ASUROfileName[1024];

	unsigned int m_TimeoutFlash;	// sec
	unsigned int m_TimeoutConnect;  // sec
	unsigned int m_MaxTry;
        unsigned int m_Incremental;

	Flashdata m_RAM; //-1PageNo -2CRC16 
	bool m_dirty[MAX_PAGE];
        unsigned int m_TotalPages;
};
#endif
