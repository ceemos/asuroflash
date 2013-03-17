/***************************************************************************
                 hexfile.h -- parse IHex files to Asuro Pages
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

#ifndef HEXFILE_H
#define HEXFILE_H

#define MAX_PAGE  128    // AtmelMega8 8kByte Flash
#define PAGE_SIZE 64 + 3 // Bytes +1PageNo +2 CRC16
#define BOOT_SIZE 1024   // Bytes

#define HEX_HEADER 1+2+4+2+2 // : + recordLength + address + type + chksum

typedef unsigned char Flashdata[MAX_PAGE][PAGE_SIZE - 3]; //-1PageNo -2CRC16 
bool ParseHex(char* file, Flashdata m_Ram, bool m_dirty[]);
char readLine(char* line, FILE *fp);
#endif