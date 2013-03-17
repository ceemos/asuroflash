/***************************************************************************
                               PosixSerial.h
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
#ifndef POSIXSERIAL_H
#define POSIXSERIAL_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#if defined LINUX || defined _LINUX || defined _LINUX_
#define LINUX
#endif

#ifdef _QT
#include <qmessagebox.h>
#endif
#include "Serial.h"

class CPosixSerial : public CSerial
{
public:
	CPosixSerial();
	virtual ~CPosixSerial();
	bool Open(char* port);
	void Close(void);
  bool Scan(char* port, unsigned short number, unsigned short mode);
	void ClearBuffer(void);
	int Read(char* data, unsigned int length);
	int Write(char* data, unsigned int length);
	void Timeout(unsigned int timeout); //msec

protected :
  int m_portHandle;
  struct termios CommConfig;
	
private :
	void MyMessageBox(char* Text);
	char m_portName[256];

};
#endif

