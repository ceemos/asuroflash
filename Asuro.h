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

#include <stdio.h>

#define START_PAGE 0x0000
#define END_PAGE   0x2000

#define MAX_PAGE  128    // AtmelMega8 8kByte Flash
#define PAGE_SIZE 64 + 3 // Bytes +1PageNo +2 CRC16
#define BOOT_SIZE 1024   // Bytes

#define HEX_HEADER 1+2+4+2+2 // : + recordLength + address + type + chksum

static const char*  ASUROAbout = "ASURO Flash Tool\n\
Version 1.2\n\
Author: Jan Grewe\n\
(c)DLR 2003-20004";
static const char*  LICENSE = "ASURO Flash Copyright (c)2003-2004 DLR RM\n\
ASURO Flash comes with\n\
ABSOLUTELY NO WARRANTY \n\
This program is free software\n\
you can redistribute it and/or modify\n\
it under the terms of the\n\
GNU General Public License\n\
as published by\n\
the Free Software Foundation\n\
either version 2 of the License\n\
or any later version\n";

class CAsuro  
{
public:
	void Programm (void);
	bool InitCAsuro(void);
  bool PortScan(char*,unsigned short, unsigned short);
	CAsuro();
	virtual ~CAsuro();

	virtual void Status(char*) {};
	virtual void MessageText(char*) {};
	virtual void WarningText(char*) {};
	virtual void SuccessText (char*) {};
	virtual void ErrorText(char*) {};
	virtual void Progress(unsigned int) {};
	virtual void ViewUpdate(void) {};
	virtual void TimeWait(unsigned int time) = 0; // msec

private:
	bool SendPage(void);
	bool BuildRAM(void);
	bool Connect(void);
	bool Init(void);
	unsigned int CRC16 (unsigned int crc, unsigned char data);
	char readLine(char* line, FILE* fp);

public:
	bool m_ASUROCancel;
	char m_ASUROCOMPort[255];
	char m_ASUROfileName[1024];
	char m_ASUROIniPath[1024];

private:
	unsigned int m_TimeoutFlash;	// sec
	unsigned int m_TimeoutConnect;  // sec
	unsigned int m_MaxTry;

	unsigned int m_startPage,m_endPage;
	unsigned char m_RAM[MAX_PAGE][PAGE_SIZE - 3]; //-1PageNo -2CRC16 
};
#endif
