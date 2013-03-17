/***************************************************************************
             PosixSerial.cpp - Serial Communictaion in Posix standard
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
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "PosixSerial.h"

CPosixSerial::CPosixSerial()
{
	
}

CPosixSerial::~CPosixSerial()
{

}

bool CPosixSerial::Open(char* port)
{
char text[256];

#ifdef LINUX
/*
#elif defined(Q_OS_IRIX)  || defined(_OS_IRIX_)
	sprintf(portName,"/dev/ttyf%d",port+1);
#elif defined(Q_OS_HPUX) || defined(_OS_HPUX_)
	sprintf(portName,"/dev/tty1p%d",port);
#elif defined(Q_OS_SOLARIS) || defined(_OS_SOLARIS_)
	sprintf(portName,"/dev/ttyS%d",port);
#elif defined(Q_OS_ULTRIX) || defined(_OS_ULTRIX_)
	sprintf(portName,"/dev/tty%02d",port+1);
*/
#else
#error Wrong OS only LINUX implemented
#endif
  strcpy(m_portName,port);
  m_portHandle = open ((const char*)m_portName, O_RDWR | O_NOCTTY);
	if (m_portHandle == -1) {
		sprintf(text,"Could not open %s\nAlready in use ?!?!\n",m_portName);
		MyMessageBox(text);
		return false;
	}
  // configure port settings
  tcgetattr(m_portHandle, &CommConfig);

	// 2400 Baud / Data Size 8-Bit / 1 Stop Bit / No Parity / No Flow Control / Zero TimeOut
  CommConfig.c_cflag = CREAD|CLOCAL|B2400|CS8;
  CommConfig.c_lflag = 0;
  CommConfig.c_oflag = 0;
  CommConfig.c_iflag = 0;
  CommConfig.c_cc[VMIN] = 0;
	CommConfig.c_cc[VTIME]= 0;

 
  // Set DTR & RTS
  ioctl(m_portHandle, TIOCMSET, TIOCM_DTR | TIOCM_RTS);
  
  if (tcsetattr(m_portHandle, TCSAFLUSH, &CommConfig)) {
 		sprintf(text,"Can't write port settings on %s\n",m_portName);
		MyMessageBox(text);
		return false;
  } 	
  return true;
}

void CPosixSerial::Close(void)
{
	close(m_portHandle);
}

bool CPosixSerial::Scan(char* port,unsigned short number,unsigned short mode)
{
int ret = false;
  if (mode == SERIAL) sprintf(m_portName,"/dev/ttyS%d",number);
  if (mode == USB) sprintf(m_portName,"/dev/ttyUSB%d",number);
  m_portHandle = open ((const char*)m_portName, O_RDWR | O_NOCTTY);
  if (m_portHandle != -1) {
    if(!(tcgetattr(m_portHandle, &CommConfig))) {
      strcpy(port,m_portName);
      ret = true;
    }
  }
  Close();
  return ret;
}

void CPosixSerial::ClearBuffer(void)
{
//  tcflush(m_portHandle,TCIFLUSH);
//  tcflush(m_portHandle,TCOFLUSH);
//  tcflush(m_portHandle,TCIOFLUSH);

}


int CPosixSerial::Read(char* data, unsigned int length)
{
	int ret;
	char text[256];
	ret = read(m_portHandle,data,length);
	if (ret == -1) {
		sprintf(text,"Can't read from %s",m_portName);
		MyMessageBox(text); 
	} 
	return ret;	
}

int CPosixSerial::Write(char* data, unsigned int length)
{
	int ret;
	char text[256];
	ret = write(m_portHandle,data,length);
	if (ret == -1) {
		sprintf(text,"Can't write to %s",m_portName);
		MyMessageBox(text);
	}
	return ret; 
}

void CPosixSerial::Timeout(unsigned int timeout) //msec
{
  CommConfig.c_cc[VTIME]=timeout/100;
  tcsetattr(m_portHandle, TCSAFLUSH, &CommConfig);
}

void CPosixSerial::MyMessageBox(char* Text)
{
#ifdef _QT 
	QMessageBox::critical(NULL,"ASURO Flash",Text,"Quit",0);
#elif defined _CONSOLE
	std::cout << Text << std::endl;
#else
#error Must be QT or CONSOLE Applikation
#endif
}
	
