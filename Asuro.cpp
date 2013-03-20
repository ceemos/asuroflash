/***************************************************************************
             Asuro.cpp  -  Main Class for ASURO Flash Tools
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
 *   any later version.                                                    *
 ***************************************************************************/

/***************************************************************************
	Ver      date       Author        comment
  -------------------------------------------------------------------------
	1.0   12.08.2003    Jan Grewe     build
	1.1	  11.12.2003    Jan Grewe     fixed error m_endPage
	1.2   27.08.2004    Jan Grewe     Port Scan included
 ***************************************************************************/ 


#include <string.h>
#include <time.h>
#include <stdio.h>
#include <fstream>

#include "PosixSerial.h"
#include "hexfile.h"

#include "Asuro.h"
CPosixSerial Serial;

CAsuro::CAsuro()
{
	memset(m_ASUROfileName,0x00,sizeof(m_ASUROfileName));
	memset(m_ASUROCOMPort,0x00,sizeof(m_ASUROCOMPort));
	m_ASUROCancel = false;
	m_MaxTry = 10;
	m_TimeoutConnect = 10;
	m_TimeoutFlash = 1;
	m_Incremental = 1;
}


bool CAsuro::LoadIni(char* inifile)
{
	FILE* fp = NULL;
	char line[sizeof(m_ASUROfileName)],lineCount = 0;

	if (inifile[0] != '\0') fp = fopen(inifile,"r");
	if (fp != NULL) {

		rewind(fp);
		while (readLine(line,fp) != EOF) {
			switch (lineCount) {
				case 0: memcpy(m_ASUROfileName,line,strlen(line) -1); break;
				case 1: memcpy(m_ASUROCOMPort,line,strlen(line) -1); break;
				case 2: sscanf(line,"%d",(int *)&m_TimeoutConnect); break;
				case 3: sscanf(line,"%d",(int *)&m_TimeoutFlash); break;
				case 4: sscanf(line,"%d",(int *)&m_MaxTry); break; 
				case 5: sscanf(line,"%d",(int *)&m_Incremental); break; 
			}
			lineCount ++;
		}
		fclose(fp);
		return true;
	}
	return false;
}

void CAsuro::SaveIni(char* inifile)
{
	FILE* fp = NULL;
	if (inifile[0] != '\0') fp = fopen(inifile,"w");
	if (fp != NULL) {
		
		if (m_ASUROfileName == "") fprintf(fp,"Hex Filename \n");
		else fprintf(fp,"%s\n",m_ASUROfileName);
		
 		if (m_ASUROCOMPort == "") fprintf(fp,"Interface \n");
		else fprintf(fp,"%s\n",m_ASUROCOMPort);
		
		fprintf(fp,"%d \t\t#Timeout Connect\n",m_TimeoutConnect);
		fprintf(fp,"%d \t\t#Timeout Flash ('t')\n",m_TimeoutFlash);
		fprintf(fp,"%d \t\t#MaxTry for flashing\n",m_MaxTry);
		fprintf(fp,"%d \t\t#Flash incremental\n",m_Incremental);
		fclose(fp);
	}
}

bool CAsuro::Init()
{
	if (!Serial.Open(m_ASUROCOMPort)) {
		ErrorText("Failed !");
		return false;
	}
	Serial.Timeout(0);
	SuccessText("OK !");
	return true;
}

bool CAsuro::Connect()
{
	char buf[11] = {0,0,0,0,0,0,0,0,0,'\0'};
	Serial.ClearBuffer();
	time_t t1,t2;

	time(&t1);
	time(&t2);
	MessageText("Connect to ASURO --> ");
	while (difftime(t2,t1) < m_TimeoutConnect) {
		time(&t2);
		if (m_ASUROCancel) {
			ErrorText("Cancel !");	
			return false;
		}
		Progress((unsigned int)(difftime(t2,t1) * 100.0) / m_TimeoutConnect);
		Serial.Write((char*)"Flash",5);
		Serial.Read(buf,10);
		TimeWait(100);
		ViewUpdate();
		if (strstr(buf,"ASURO")) {
			SuccessText("OK !");
			return true;
		}
	}
	Progress(100);
	ErrorText("Timeout !");
	return false;
}

bool CAsuro::BuildRAM()
{
	bool res = ParseHex(m_ASUROfileName, m_RAM, m_dirty);
	if (!res) return false;
	
	int newpages = 0;
	for (int i = 0; i < MAX_PAGE; i++) {
		if (m_dirty[i]) newpages++;
	}
	
	if (m_Incremental) {
		Flashdata oldflash;
		bool olddirty[MAX_PAGE];
		memset(olddirty, 0, sizeof(olddirty));
		memset(oldflash, 0, sizeof(oldflash));
		res = ParseHex((char*)TMPFILE, oldflash, olddirty);
		if (res) { // compare page by page for changes
			for (int i = 0; i < MAX_PAGE; i++) {
				if (olddirty[i] && m_dirty[i] && memcmp(oldflash[i], m_RAM[i], sizeof(oldflash[i])) == 0)
					m_dirty[i] = false;
			}
		}
	}
	
	m_TotalPages = 0;
	for (int i = 0; i < MAX_PAGE; i++) {
		if (m_dirty[i]) m_TotalPages++;
	}
	
	char tmp[64];
	sprintf(tmp,"Updating %d of %d pages.",m_TotalPages, newpages);
	MessageText(tmp);
	
	return true;
}

bool CAsuro::SendPage(unsigned int number)
{
	unsigned int crc,i,j;
	char sendData[PAGE_SIZE]; 
	char getData[3],tmpText[256];
	time_t t1,t2;
	i = number;
	Serial.Timeout(100);
	sendData[0] = i; // PageNo.
	crc = 0;
	memcpy(&sendData[1],&m_RAM[i][0],PAGE_SIZE - 3);
	//Build CRC16
	for (j = 0; j < PAGE_SIZE - 2; j++) // -2 CRC16
		crc = CRC16(crc,sendData[j]);
	memcpy(&sendData[j],&crc,2);

	// Try MAX_TRY times before giving up sendig data
	for (j = 0; j < m_MaxTry; j++) {
		memset(getData,0x00,3);
		Serial.Write(sendData,PAGE_SIZE);
		Serial.ClearBuffer();
		
		time(&t1);
		time(&t2);
		// read one extra byte. The loop will read all sent data, and an odd 
		// number of bytes ist sent, so data won't align otherwise.
		Serial.Read(getData,1);
		int ctr = PAGE_SIZE / 2;
		do {
			time(&t2);
			if (m_ASUROCancel == true) {
				ErrorText("Cancel!");
				return false;
			}
			
			int count = Serial.Read(getData,2);
			if (count < 2)  // read the second byte if missing
				Serial.Read(getData+1,1);
			
			Serial.ClearBuffer();
			ViewUpdate();
			
#ifdef DEBUG
			for (int k = 0; k < 2; k++)
				if (getData[k] > 'A' && getData[k] < 'Z') fprintf(stderr, "%c", getData[k]);
				else fprintf(stderr, ".");
			fprintf(stderr, " ");
#endif
			
		} while (	ctr-- > 0 || (
					(strcmp(getData,"CK") != 0) &&
					(strcmp(getData,"OK") != 0) &&
					(strcmp(getData,"ER") != 0) &&
					difftime(t2,t1) <= m_TimeoutFlash));
		TimeWait(50);
		if (getData[0] == 'O' && getData[1] == 'K') {
			SuccessText(" flashed !");
			return true; // Page sended succssesfull
		}
		if (getData[0] == 'C' && getData[1] == 'K') WarningText("c"); 
		else if (getData[0] == 'E' && getData[1] == 'R') WarningText("v");
		else WarningText("t");  
	}

	if (j == m_MaxTry) {
		MessageText(""); // just for \n
		ErrorText("TIMEOUT !");	
		MessageText(""); // just for \n
		ErrorText(" ASURO dead --> FLASH damaged !!");
		return false;
	}

	return false; // should not be reached
}

void CAsuro::FinalPage() {
	char sendData[PAGE_SIZE]; 
	memset(sendData,0xFF,PAGE_SIZE);
	Serial.Write(sendData,PAGE_SIZE);
	Serial.ClearBuffer();
}


void CAsuro::Programm()
{
	char tmp[255];
	int ctr = 0;
	m_ASUROCancel = false;
	sprintf(tmp,"Try to initialise %s ",m_ASUROCOMPort);
	Status(tmp);
	sprintf(tmp,"Open %s --> ",m_ASUROCOMPort);  
	MessageText(tmp);
	ViewUpdate();
	bool res;
	res = Init();
	if(!res) {
		sprintf(tmp,"Can't initialise %s !",m_ASUROCOMPort);
		Status(tmp);
		goto cleanup;
	}
	Status("Building  RAM !" );
	ViewUpdate();
	res = BuildRAM();
	if(!res) {
		Status("Building  RAM failed !" );
		goto cleanup;
	}
	Status("Try to connect ASURO !" );
	ViewUpdate();
	Progress(0);
	res = Connect();
	if(!res) {
		Status("Connect to ASURO failed !");
		goto cleanup;
	}
	Status("Flashing Firmware !" );
	ViewUpdate();
	
	for(int i = 0; i < MAX_PAGE; i++) {
		if (m_dirty[i]) {
			sprintf(tmp,"Sending Page %03d of %03d --> ",ctr++,m_TotalPages);
			MessageText(tmp);
			res = SendPage(i);
			if(!res) break;
		}
	}
	
	FinalPage();
	MessageText(""); // just for \n
	SuccessText("All Pages flashed !!");
	
	if (!res) {
		Status("ASURO dead ?!?! (Firmware damaged; try again!)" );
		if (m_Incremental) { // Might be outdated
			unlink(TMPFILE);
		}
	} else {
		Status("ASURO ready to start!" );
		if (m_Incremental) { // Save hexfile
			std::ifstream source(m_ASUROfileName, std::ios::binary);
			std::ofstream dest(TMPFILE, std::ios::binary);
			dest << source.rdbuf();
			source.close();
			dest.close();
		}
	}
	
cleanup:
	Serial.Close();
}


unsigned int CAsuro::CRC16(unsigned int crc, unsigned char data)
{
const unsigned int CRCtbl[ 256 ] = {                                 
   0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,   
   0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,   
   0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,   
   0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,   
   0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,   
   0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,   
   0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,   
   0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,   
   0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,   
   0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,   
   0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,   
   0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,   
   0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,   
   0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,   
   0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,   
   0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,   
   0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,   
   0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,   
   0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,   
   0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,   
   0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,   
   0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,   
   0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,   
   0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,   
   0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,   
   0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,   
   0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,   
   0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,   
   0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,   
   0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,   
   0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,   
   0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 }; 

   return ( crc >> 8 ) ^ CRCtbl[ ( crc & 0xFF ) ^ data];     
}




// kate: tab-width 4; indent-width 4; space-indent off;