/***************************************************************************
                    ANSIAsuro.cpp  -  Interface ASURO Console Flash
                             -------------------
    begin                : Die Aug 12 10:16:57 CEST 2003
    copyright            : (C) 2003 DLR RM by Jan Grewe
    email                : jan.grewe@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 +   any later version.                                                    *
 ***************************************************************************/

/***************************************************************************
	Ver      date       Author        comment
	--------------------------------------------------------------------------
	1.0   12.08.2003    Jan Grewe     build
 ***************************************************************************/ 
#include "ANSIAsuro.h"
#ifdef WINDOWS
#include "stdafx.h"
#endif
#ifdef LINUX
#include <unistd.h>
#endif
#include <iostream>


CANSIAsuro::CANSIAsuro()
{

}

CANSIAsuro::~CANSIAsuro()
{

}

void CANSIAsuro::Progress(unsigned int progress)
{
 std::cout << "." << std::flush;
}

void CANSIAsuro::TimeWait(unsigned int time) // msec
{
#ifdef WINDOWS
	Sleep(time);
#elif defined LINUX
	usleep(time * 1000);
#endif
}

void CANSIAsuro::MessageText(char* text)
{
	std::cout << std::endl << text << std::flush;
}

void CANSIAsuro::WarningText(char* text)
{
	std::cout << text << std::flush;
}

void CANSIAsuro::SuccessText (char* text)
{
	std::cout << text << std::flush;
}

void CANSIAsuro::ErrorText(char* text)
{
	std::cout << text << std::flush;
}
