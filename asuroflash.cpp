/***************************************************************************
             Con_Flash.cpp  -  Console Flash Applikation for ASURO
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

#include "ANSIAsuro.h"
#ifdef WINDOWS
#include "stdafx.h"
#endif

#include <stdio.h>
#include <string.h>

#include <iostream>

void showHelp(void);

CANSIAsuro Asuro;

int main(int argc, char* argv[])
{
  std::cout << LICENSE << std::endl;
	std::cout << std::endl << ASUROAbout << std::endl;
#ifdef WINDOWS
	std::cout << "Microsoft Version" << std::endl << std::endl;
	char* tmp = strrchr(argv[0],'\\');
	strncpy(Asuro.m_ASUROIniPath,argv[0],strlen(argv[0]) - strlen(tmp) + 1);
#elif defined LINUX
	std::cout << "Linux Version" << std::endl << std::endl;
	Asuro.m_ASUROIniPath[0] = '\0';
#else
#error OS not implemented (only LINUX, WINDOWS)
#endif
	strcat(Asuro.m_ASUROIniPath,"AsuroFl.ini");

	if (argc > 3) {
		std::cout << "too many arguments !!" << std::endl << std::endl;
		std::cout << "usage : " << argv[0]; 
		showHelp();
		// Write no ini File
		Asuro.m_ASUROIniPath[0] = '\0';
		return -1;
	}
	if (argc == 2) {
		std::cout << "Asuro Flash help " << std::endl << std::endl;
		std::cout << "usage : " << argv[0]; 
		showHelp();
		// Write no ini File
		Asuro.m_ASUROIniPath[0] = '\0';
		return -1;
	}

	if (argc == 1) {
		if (Asuro.InitCAsuro() == false) {
			std::cout << "Can't find ini file !!" << std::endl << std::endl;
			std::cout << "usage : " << argv[0]; 
			showHelp();
			// Write no ini File
			Asuro.m_ASUROIniPath[0] = '\0';
			return -1;
		}
		else {
#ifdef LINUX
			std::cout << "Start ASURO Flash on " << Asuro.m_ASUROCOMPort << 
			" flashing " << Asuro.m_ASUROfileName << std::endl;
#elif defined WINDOWS
			std::cout << "Start ASURO Flash on " << Asuro.m_ASUROCOMPort << 
			" flashing " << Asuro.m_ASUROfileName << std::endl;
#endif
			Asuro.InitCAsuro();
		}
	}
	if (argc == 3) { 
		strcpy(Asuro.m_ASUROCOMPort,argv[1]);
		strcpy(Asuro.m_ASUROfileName,argv[2]);
	}
	Asuro.Programm();
	std::cout << std::endl << "Press RETURN to Exit" << std::endl;
	while(!getchar());
	return 0;
}

void showHelp(void)
{
	std::cout << " [SerialPort] [filename]" << std::endl;
	std::cout << "no options : try to read parameter from .ini file" << std::endl; 
#ifdef LINUX
	std::cout << "SerialPort : /dev/ttyS0 /dev/ttyS1 /dev/ttyUSB0 ..." << std::endl;
#elif defined WINDOWS
	std::cout << "SerialPort : COM1 COM2 ..." << std::endl;
#endif
	std::cout << "filename   : complete Path to *.hex file which should be flashed" << std::endl;
	std::cout << "if all parameter are checked .ini file will be generated" << std::endl << std::endl; 
	std::cout << std::endl << "Press RETURN to Exit" << std::endl;
	while (!getchar());
}
