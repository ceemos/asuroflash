/***************************************************************************
                               ANSIAsuro.h
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
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
	Ver      date       Author        comment
	--------------------------------------------------------------------------
	1.0   12.08.2003    Jan Grewe     build
 ***************************************************************************/
#ifndef ANSIASURO_H
#define ANSIASURO_H

#include "Asuro.h"

class CANSIAsuro : public CAsuro  
{
public:
	CANSIAsuro();
	virtual ~CANSIAsuro();
	void MessageText(char* text);
	void WarningText(char* text);
	void SuccessText (char* text);
	void ErrorText(char* text);
	void Progress(unsigned int progress);
	void TimeWait(unsigned int time); // msec

	char m_iniPathName[1024];
};
#endif
