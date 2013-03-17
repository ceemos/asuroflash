/***************************************************************************
                 hexfile.cpp -- parse IHex files to Asuro Pages
                           -------------------
    begin               : Sun Mar 17 16:21:05 CET 2013

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include "hexfile.h"

void message(std::string text) {
	std::cout << text << std::endl;
}
#define MessageText message
#define ErrorText message
#define SuccessText message

bool ParseHex(char* file, Flashdata m_RAM, bool m_dirty[]) {
	FILE *fp = NULL;
	unsigned int address = 0, type = 0, data = 0, cksum = 0, cksumFile = 0, recordLength = 0, i;
	char tmp[256],line[1024];

	
	MessageText("Bulding  RAM --> ");
	if (file[0] != '\0') fp = fopen(file,"r");

	if (fp != NULL) {
		rewind(fp);
		while (readLine(line,fp) != EOF) {
			if (line[0] != ':') {
				fclose(fp);
				ErrorText("Wrong file format !");
				return false;
			}
			cksum = 0;
			sscanf(&line[1],"%02X",&recordLength);
			sscanf(&line[3],"%04X",&address);
			sscanf(&line[7],"%02X",&type);
			// get page number
			unsigned int pgnum = (unsigned int) ((address / (PAGE_SIZE - 3)) +0.5);
			m_dirty[pgnum] = true;
			cksum = recordLength +(unsigned char) ((address & 0xFF00) >> 8)
							+ (unsigned char) (address & 0x00FF) + type;
			
			if (type == 0x00) { // data Record
				int header = HEX_HEADER;
#ifdef WINDOWS 
				// Windows \n\r = 1 char as EOL
				if (line[strlen(line)-1] == 0x0a) header ++;
#else
				// Unix \n = 1 char as EOL reading Windows file with \n\r as EOL is 2 char
				if (line[strlen(line)-1] == 0x0a) header += 2;
#endif
				if (strlen(line) != recordLength * 2 + header) { 
					ErrorText("HEX file line length ERROR !");
					return false;
				} 

				for ( i = 0; i < recordLength; i++) {
					sscanf(&line[9 + i*2],"%02X",&data);
					cksum += data;
					tmp[i] = data;
				}
				cksum = ((~cksum & 0xFF) + 1) & 0xFF;
				sscanf(&line[9 + i*2],"%02X",&cksumFile);
				if (cksum != cksumFile) {
					fclose(fp);
					ErrorText("HEX file chksum ERROR !");
					return false;
				}

				if (address + recordLength > (MAX_PAGE * PAGE_SIZE) - BOOT_SIZE) {
					fclose(fp);
					ErrorText("Hex file too large or address error !!");
					return false;
				}
				memcpy(&m_RAM[0][0]+address,&tmp[0],recordLength);
			}
			if (type == 0x01) { // End of File Record
				SuccessText("OK !");
				fclose(fp);
				return true;
			}
		}
	}
	sprintf(line,"%s does not exist !",file);
	ErrorText(line);
	return false;
}

char readLine(char* line, FILE *fp)
{
	char c;
	unsigned int i = 0;
	do {
		c = fgetc(fp);
		if (c == EOF) 
			return EOF;
		line[i++] = c;
	} while (c != '\n');
	line[i] = '\0';
	return true;
}

// kate: tab-width 4; indent-width 4; space-indent off;