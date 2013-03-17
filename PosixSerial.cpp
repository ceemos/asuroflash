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

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include "PosixSerial.h"

bool CPosixSerial::Open(char* port)
{

  strcpy(m_portName,port);
  m_portHandle = open ((const char*)m_portName, O_RDWR | O_NOCTTY);
  if (m_portHandle == -1) {
    fprintf(stderr, "Could not open %s\nAlready in use ?!?!\n",m_portName);
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
    return false;
  } 
  return true;
}

void CPosixSerial::Close(void)
{
	close(m_portHandle);
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
		fprintf(stderr, "Can't read from %s",m_portName);
	} 
	return ret;
}

int CPosixSerial::Write(char* data, unsigned int length)
{
	int ret;
	char text[256];
	ret = write(m_portHandle,data,length);
	if (ret == -1) {
		fprintf(stderr,"Can't write to %s",m_portName);
	}
	return ret; 
}

void CPosixSerial::Timeout(unsigned int timeout) //msec
{
  CommConfig.c_cc[VTIME]=timeout/100;
  tcsetattr(m_portHandle, TCSAFLUSH, &CommConfig);
}
