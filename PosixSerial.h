/***************************************************************************
                               PosixSerial.h
                           -------------------
    begin               : Die Aug 12 10:16:57 CEST 2003
    copyright       : (C) 2003-2004 DLR RM by Jan Grewe
    email               : jan.grewe@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef POSIXSERIAL_H
#define POSIXSERIAL_H

#if defined LINUX || defined _LINUX || defined _LINUX_
#define LINUX
#endif
#include <termios.h>


class CPosixSerial
{
public:
	bool Open(char* port);
	void Close(void);
	void ClearBuffer(void);
	int Read(char* data, unsigned int length);
	int Write(char* data, unsigned int length);
	void Timeout(unsigned int timeout); //msec
        int m_portHandle;
        struct termios CommConfig;

	char m_portName[256];

};
#endif

