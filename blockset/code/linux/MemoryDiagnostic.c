/************************************************************************************//**
* \file         MemoryDiagnostic.c
* \brief        Handle the reading and storing of diagnostic codes.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h> /* For dir handling */

#include "MemoryDiagnostic.h"

static int remove_directory(const char *path);

/***************************************************************************************
** \brief	Function that needs to be called once during initialze to create the folder
**			to store diagnostic trouble codes
** \param	None
** \return	None.
**
****************************************************************************************/
void MemoryDiagnostic_InitializeMemory(void)
{
	/* Check if folder is already available. If not, create one */
	struct stat st = {0};

	if (stat("/usr/mem-diag", &st) == -1) {
	mkdir("/usr/mem-diag", 0555);
	}
}


/***************************************************************************************
** \brief	Function that stores and provides the memory locations for each module
** \		data holder.
** \param	Module slot on which the module is installed to. Value from 1 to 8
** \return	If requested, the memory location of the modules data holder.
**
****************************************************************************************/
void MemoryDiagnostic_WriteToMemory(uint32_t spn, uint8_t fmi, uint8_t oc, char* freezedDescription, float freezedParameter, uint8_t messageType)
{
	/* Write string to file */
	static int fileId = 0;
	static uint32_t diagnosticCode;

	/* Open file to write data to */
	if(messageType == DIAGNOSTICSTART)
	{
	/* Create diagnostic code to store */
	diagnosticCode = (spn&0xffff); //1234
	diagnosticCode += (spn&0x70000) << 5; //1234
	diagnosticCode += (fmi&0x1f) << 16; //2
	diagnosticCode += (oc&0x7f) << 24; //1
		
	char path[50] = {0};
	/* Create string from diagnostic code */
	char diagnosticCodeStr[15];
	sprintf(diagnosticCodeStr, "%u",(int)diagnosticCode);
	/* Combine strings to correct file naming */
	strcat(path, "/usr/mem-diag/");
	strcat(path, diagnosticCodeStr);
	/* Create diagnostic error code file */
	fileId = open(path, O_WRONLY | O_CREAT | O_NONBLOCK  | O_TRUNC, 0555);
	}

	/* Write the data to the file*/
	if(messageType == DIAGNOSTICFREEZE)
	{
	char codeStr[50] = {0};
	/* Create string from value */
	char valueString[15];
	sprintf(valueString, "%1.2f",freezedParameter);
	/* Combine strings to correct freezed parameter */
	strcat(codeStr, freezedDescription);
	strcat(codeStr, " : ");
	strcat(codeStr, valueString);
	strcat(codeStr, "\n");
	/* Create diagnostic error code file */
	write(fileId, codeStr, strlen(codeStr));
	}

	/* Close the file descriptor */
	if(messageType == DIAGNOSTICSTOP)
	{
	write(fileId, "\n", strlen("\n"));
	close(fileId);
	}
}

/***************************************************************************************
** \brief	Function that counts the number of DTC's stored in /usr/mem-diag folder 
** \param	None
** \return	Number of stored DTC's
**
****************************************************************************************/
uint16_t MemoryDiagnostic_CountDiagCodes(void)
{
uint16_t file_count = 0;
DIR * dirp;
struct dirent * entry;

	char path[20] = {0};
	/* Construct properpath name */
	strcat(path, "/usr/mem-diag/");

	dirp = opendir(path); /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_type == DT_REG) { /* If the entry is a regular file */
			 file_count++;
		}
	}
	closedir(dirp);
	
return file_count;
}

/***************************************************************************************
** \brief	Delete all stored codes from /usr/mem-diag folder
** \param	None
** \return	None
**
****************************************************************************************/
void MemoryDiagnostic_DeleteAllCodes(void)
{
	char path[20] = {0};
	strcat(path, "/usr/mem-diag/");
	/* Remove the actual folder */
	if(remove_directory(path) ==0)
	{
		/* Recreate the path */
		mkdir(path, 0555);
	}
}


/***************************************************************************************
** \brief	Function that deletes only one specific diagnostic code from /usr/mem-sim folder
** \param	spn number
			fmi regarding J1939
			oc which need to be set to one by default
** \return	None
**
****************************************************************************************/
void MemoryDiagnostic_DeleteSingleCode(uint32_t spn, uint8_t fmi, uint8_t oc)
{
	static uint32_t diagnosticCode;
	
	/* Create diagnostic code to store */
	diagnosticCode = (spn&0xffff); //1234
	diagnosticCode += (spn&0x70000) << 5; //1234
	diagnosticCode += (fmi&0x1f) << 16; //2
	diagnosticCode += (oc&0x7f) << 24; //1
	
	/* Create string from diagnostic code */
	char diagCodeStr[20];
	sprintf(diagCodeStr, "%u",(int)diagnosticCode);
	/* Combine string to construct proper path name */
	char file[35] = {0};
	strcat(file, "/usr/mem-diag/");
	strcat(file, diagCodeStr);
	/* Remove the actual folder */
	if(remove(file) ==0)
	{
		/* Recreate the path */
		//mkdir(path, 0555);
	}
}


/***************************************************************************************
** \brief	Function that stores and provides the memory locations for each module
** \		data holder.
** \param	Module slot on which the module is installed to. Value from 1 to 8
** \return	If requested, the memory location of the modules data holder.
**
****************************************************************************************/
static int remove_directory(const char *path) {
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d) {
      struct dirent *p;

      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;

          len = path_len + strlen(p->d_name) + 2;
          buf = malloc(len);

          if (buf) {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = remove_directory(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }

   if (!r)
      r = rmdir(path);

   return r;
}


/***************************************************************************************
** \brief	Function that stores and provides the memory locations for each module
** \		data holder.
** \param	diagType diagnostic type, a value from 1 to 5
** \param	index
** \return	If requested, the memory location of the modules data holder.
**
****************************************************************************************/
uint32_t MemoryDiagnostic_DiagCodeOnIndex(uint16_t index)
{
		/* Construct proper path name */
		char path[20] = {0};
		strcat(path, "/usr/mem-diag/");

		DIR *d;
		struct dirent *dir;
		d = opendir(path);

		uint16_t indexCounter = 0;

		if (d) {
			while ((dir = readdir(d)) != NULL) {

				/* Skip the names "." and ".." as we don't want to recurse on them. */
				if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")){
				continue;
				}
				/* Check if the actual file location meets the index number */
				if(indexCounter == index)
				{
					/* At this point we have a match on index */
					uint32_t code = strtol(dir->d_name, NULL, 10);
					closedir(d);
					/* Debug purpose */
					//printf("code: %d\n",code);
					return code;
				}
				indexCounter++;
			}
			closedir(d);
		}
	return(0);
}