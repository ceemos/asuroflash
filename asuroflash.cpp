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

#include "Asuro.h"
#ifdef WINDOWS
#include "stdafx.h"
#endif

#include <stdio.h>
#include <string.h>

#include <iostream>


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
	std::cout << ASUROAbout << std::endl;
	std::cout << LICENSE << std::endl;
}

CAsuro Asuro;

int main(int argc, char* argv[])
{
	char inifile[1024];
	inifile[0] = '\0';
#ifdef WINDOWS
	char* tmp = strrchr(argv[0],'\\');
	strncpy(Asuro.m_ASUROIniPath,argv[0],strlen(argv[0]) - strlen(tmp) + 1);
#endif
	strcat(inifile,"AsuroFl.ini");

	if (argc == 2 || argc > 3) {
		std::cout << "Asuro Flash help " << std::endl << std::endl;
		std::cout << "usage : " << argv[0]; 
		showHelp();
		return -1;
	}

	if (argc == 1) {
		if (Asuro.LoadIni(inifile) == false) {
			std::cout << "Can't find ini file!" << std::endl << std::endl;
			std::cout << "usage : " << argv[0]; 
			showHelp();
			return -1;
		}
		else {
			std::cout << "Start ASURO Flash on " << Asuro.m_ASUROCOMPort << 
			" flashing " << Asuro.m_ASUROfileName << std::endl;
		}
	}
	if (argc == 3) { 
		strcpy(Asuro.m_ASUROCOMPort,argv[1]);
		strcpy(Asuro.m_ASUROfileName,argv[2]);
		Asuro.m_Incremental = 1;
	}
	Asuro.Programm();
	std::cout << "Done." << std::endl;
	Asuro.SaveIni(inifile);
	return 0;
}

// kate: tab-width 4; indent-width 4; space-indent off;