/***************************************************************************
                               Serial.h
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
#ifndef _SERIAL_H_
#define _SERIAL_H_

#define SERIAL  0
#define USB     1

class CSerial  
{
public:
	CSerial();
	virtual ~CSerial();
	virtual bool Open(char* port) = 0;
	virtual void Close(void) = 0;
  virtual bool Scan(char* port, unsigned short number, unsigned short mode) = 0;
	virtual void ClearBuffer(void) = 0;
	virtual int Read(char* data, unsigned int length) = 0;
	virtual int Write(char* data, unsigned int length) = 0;
	virtual void Timeout(unsigned int timeout) = 0; //msec 
	char m_Port[255];
};

#endif
