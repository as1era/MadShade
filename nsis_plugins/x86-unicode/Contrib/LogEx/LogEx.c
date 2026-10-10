/***************************************************************************************
* FILE NAME: LogEx.c
*
* PURPOSE:
*    NSIS plugin for logging.
*    Creates a log file and writes strings to it.
*    Also able to append other files.
*
* CHANGE HISTORY
*
* $LOG$
*
* Author              Version  Date          Modifications
* J.P. de Ruiter      0.1      Feb 12 2007   Original
*    --//--           0.2      Feb 16 2007   Added comments, removed unused variables,
*                                            Fixed stack bug, added Init Parameter,
*                                            Changed the way file is read in AddFile
*    --//--           0.3      Feb 16 2007   Fixed bug with empty file in AddFile
*    --//--           0.4      Apr 26 2008   Removed newlines on ::Close
*    --//--           0.5      Oct  8 2008   Changed the Init function to open the log file in SHARE_READ mode
*    --//--           0.6      Nov 28 2008   Added Unicode support
*    --//--           0.7      Dec 30 2008   Now Unicode support is really working
*    --//--           0.7.1    Dec 31 2008   Now using the PluginCallback function, so /NOUNLOAD is no longer required
*    --//--           0.7.2    Jan  7 2009   Relinked the Unicode Plugin version with the Unicode pluginapiU.lib
*    --//--           0.8      Mar 30 2009   "false" for boolean parameters is also processed now
*    --//--           0.9      Feb  8 2011   Using string_size plug-in function variable now, fixed extra NULL char in Unicode
*    --//--           0.9.1    Jun 20 2011   Fixed possible stack corruption in Write function
*                                            Added check for valid hFile in AddFile function
***************************************************************************************/

//#define _UNICODE

#ifdef _UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "Commctrl.h"
#include "stdio.h"
#include "pluginapi.h"

#define NSIS_INST_STATUSBAR	1006
#define NSIS_INST_LISTBOX	1016

void WriteLine(TCHAR* pbuffer); // declaration of internal function

HINSTANCE g_hInstance;

HANDLE hFile;			          // Handle to the LogFile

static UINT_PTR PluginCallback(enum NSPIM msg)
{
  return 0;
}

/***************************************************************************************
* Function: LogEx::Init (exported)
* Description:
*    Open an existing or create a new file.
* Parameters:
*    - bTruncateFile: If true, truncate file, else append to existing file.
*    - FileName:      Log file name.
***************************************************************************************/
NSISFunction(Init)
{
	LOGEXDLL_INIT();
	extra->RegisterPluginCallback(g_hInstance, PluginCallback);
	{
		int nBytesDone=0;				// Number of bytes read from hAddFile
#ifdef _UNICODE
		char buf2[3];
#endif
		TCHAR buf[MAX_PATH] = _T("");	// Buffer for the log filename
		popstring(buf);					// Get the First parameter
#ifdef _UNICODE
		if(_tcscmp(buf,_T("true"))==0)
#else
		if(strcmp(buf,_T("true"))==0)
#endif
		{
			popstring(buf);		// Get Second parameter from the stack (should be the log filename)
			// Open an existing file and overwrite, or create a new file, with sharing
			hFile=CreateFile(buf,FILE_WRITE_DATA,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
#ifdef _UNICODE
			buf2[0] = (char)0xFF;
			buf2[1] = (char)0xFE;
			buf2[2] = (char)0x00;
			WriteFile(hFile,buf2,2,&nBytesDone,NULL);
#endif
		}// else open an existing file and append, or create a new file, with sharing
		else
		{
#ifdef _UNICODE
			if(_tcscmp(buf,_T("false"))==0)popstring(buf);			// Get Second Parameter
#else
			if(strcmp(buf,_T("false"))==0)popstring(buf);			// Get Second Parameter
#endif
			hFile=CreateFile(buf,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				hFile=CreateFile(buf,FILE_WRITE_DATA,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
#ifdef _UNICODE
				buf2[0] = (char)0xFF;
				buf2[1] = (char)0xFE;
				buf2[2] = (char)0x00;
				WriteFile(hFile,buf2,2,&nBytesDone,NULL);
#endif
			}
		}
		
	}
}

/***************************************************************************************
* Function: LogEx::Close (exported)
* Description:
*    Close the logfile.
* Parameters:
*    - n.a.
***************************************************************************************/
NSISFunction(Close)
{
	LOGEXDLL_INIT();
	extra->RegisterPluginCallback(g_hInstance, PluginCallback);
	{
		if(hFile!=INVALID_HANDLE_VALUE&&hFile)
		{
			CloseHandle(hFile); // Close the file
		}
	}
}

/**************************************************************************************
* Function: LogEx::Write (exported)
* Description:
*    Write a new line to the logfile.
* Parameters:
*    - bWriteToStatusList:	If equal to true, adds the string to the Status Listbox. 
*    - bWriteToStatusBar:	If equal to true, adds the string to the Status Bar. 
*    - LogString:			String to write to the logfile (max. 1000 cars)
***************************************************************************************/
NSISFunction(Write)
{
	LOGEXDLL_INIT();
	extra->RegisterPluginCallback(g_hInstance, PluginCallback);
	{
		TCHAR* buf = (TCHAR*)GlobalAlloc(GPTR, string_size * sizeof(TCHAR));			// General buffer
		int count;					// Number of lines in the Status listbox
		int bstat,blist;
		LVITEM lvi;					// Listbox structure
		HWND handle;				// Handle to the listbox/statusbar
		bstat=blist=0;
		popstring(buf);				// Get First parameter
#ifdef _UNICODE
		if(_tcscmp(buf,_T("true"))==0)// If Parameter=="true" 
#else
		if(strcmp(buf,_T("true"))==0)
#endif
		{
			blist=1;				// bWriteToStatusList=true
			buf[0]=0;				// Reset buffer
			popstring(buf);			// Get Second Parameter
#ifdef _UNICODE
			if(!_tcscmp(buf,_T("true")))// If Parameter=="true" 
#else
			if(!strcmp(buf,_T("true")))
#endif
			{
				bstat=1;			// bWriteToStatusBar=true
				buf[0]=0;			// Reset buffer
				popstring(buf);		// Get Third parameter (should be LogString)
			}                       // else Logstring=buf
#ifdef _UNICODE
			else if(_tcscmp(buf,_T("false"))==0)popstring(buf);			// Get Third Parameter
#else
			else if(strcmp(buf,_T("false"))==0)popstring(buf);			// Get Third Parameter
#endif
		}                           // else Logstring=buf
#ifdef _UNICODE
		else if(_tcscmp(buf,_T("false"))==0)popstring(buf);			// Get Second Parameter
#else
		else if(strcmp(buf,_T("false"))==0)popstring(buf);			// Get Second Parameter
#endif
		if(hFile!=INVALID_HANDLE_VALUE&&hFile)
		{
			WriteLine(buf);		        // Write the string to the logfile
			if(blist)										            // If(bWriteToStatusList)
			{
				handle = FindWindowEx (hwndParent, NULL, _T("#32770"), _T(""));	// Find the Dialog Window
				handle = GetDlgItem (handle, NSIS_INST_LISTBOX);		// Get a handle to the listbox control

				count = SendMessage (handle, LVM_GETITEMCOUNT, 0, 0);	// Get the number of lines already in the listbox

				memset (&lvi, 0, sizeof (lvi));							// Init listbox structure
				lvi.mask = LVIF_TEXT;
				lvi.iItem = count;
				lvi.pszText=buf;

				SendMessage (handle, LVM_INSERTITEM, 0, (LPARAM) &lvi);	// Add LogString to the listbox
				SendMessage (handle, LVM_ENSUREVISIBLE, (WPARAM) count, (LPARAM) FALSE); // Scroll down
			}
			if(bstat)										            // If(bWriteToStatusBar)
			{
				handle = FindWindowEx (hwndParent, NULL, _T("#32770"), _T(""));	// Find the Dialog Window
				handle = GetDlgItem (handle, NSIS_INST_STATUSBAR);		// Get a handle to the statusbar control
				SetWindowText(handle, buf);					        	// Copy LogString to the statusbar
			}
			GlobalFree(buf);
		}
	}
}

#define LOG_LENGTH 1000

/**************************************************************************************
* Function: LogEx::AddFile (exported)
* Description:
*    Append a file to the logfile.
* Parameters:
*    - ReadFromLine:	Write from line <ReadFromLine> from the file to the logfile. 
*    - ReadToLine:		Write to line <ReadToLine> from the file to the logfile. 
*    - Prefix:			Prefix to add at the beginning of each new line. (max. 32 chars)
*    - FileName:		File to add to the logfile. 
***************************************************************************************/
NSISFunction(AddFile)
{
	LOGEXDLL_INIT();
	extra->RegisterPluginCallback(g_hInstance, PluginCallback);
	{
		TCHAR addfile[OFS_MAXPATHNAME];	// Filename of the file you want to append to the logfile
		LPTSTR readbuf;					// Readbuffer
		TCHAR buf[OFS_MAXPATHNAME];		// General buffer
		TCHAR *token;					// "\r\n" token to detect a new line
		int fromline=-1;				// ReadFromLine
		int toline=-1;					// ReadToLine
		int curline=1;					// Current line
		int bWriteLine=0;				// Do we have to write the current line?
		TCHAR prefix[32];				// Prefix buffer
		HANDLE hAddFile;				// Handle to the file to add
		int nBytesRead=0;				// Number of bytes read from hAddFile
		int nBytesDone=0;				// Number of bytes written to hFile
		DWORD filesize;					// Filesize of hAddFile
#ifdef _UNICODE
     	DWORD dwLen;
#endif
		buf[0]=0;						// Reset buffer
		popstring(buf);					// Get the first parameter
#ifdef _UNICODE
		if(!_tcscmp(buf,_T("0"))||(fromline=_ttoi(buf))) // If it's an integer, copy it to fromline
#else
		if(!strcmp(buf,_T("0"))||(fromline=atoi(buf)))
#endif
		{
			buf[0]=0;					// Reset buffer
			popstring(buf);				// Get the second parameter
#ifdef _UNICODE
			if(!_tcscmp(buf,_T("0"))||(toline=_ttoi(buf))) // If it's an integer, copy it to toline
#else
			if(!strcmp(buf,_T("0"))||(toline=atoi(buf)))
#endif
			{				
				buf[0]=0;				// Reset buffer
				popstring(buf);			// Get the third parameter (the prefix)
			}							// Else the second parameter is the prefix
			else toline=-1;				// and reset toline
		}								// Else the first parameter is the prefix
		else fromline=-1;				// and reset fromline
#ifdef _UNICODE
		_tcscpy(prefix,buf);			// Copy the string to prefix
#else
		strcpy(prefix,buf);
#endif
		buf[0]=0;						// Reset buffer
		popstring(buf);					// Get the addfile string
#ifdef _UNICODE
		_tcscpy(addfile,buf);			// Copy the string to addfile
#else
		strcpy(addfile,buf);
#endif
		hAddFile=CreateFile(addfile,FILE_READ_DATA,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); // Open the file as read-only
		addfile[0]=0;					                                                // Reset buffer
		if(hFile!=INVALID_HANDLE_VALUE&&hFile&&hAddFile!=INVALID_HANDLE_VALUE&&hAddFile)// If hAddFile exists, and able to read
		{
			filesize=GetFileSize(hAddFile,NULL);										// Get size of file to add
			if(filesize!=0)																// Only read when file is not empty
			{
				readbuf=VirtualAlloc(NULL,filesize,MEM_COMMIT,PAGE_READWRITE);			// Allocate memory for filebuffer (read complete file at ones to get every single newline)
				ReadFile(hAddFile,readbuf,filesize,&nBytesRead,NULL);					// Read all Characters
#ifdef _UNICODE
				token=_tcstok(readbuf+2,_T("\r\n"));									// Find next Newline
#else
				token=strtok(readbuf,_T("\r\n"));										
#endif
				while(token!=NULL&&(curline<=toline||toline==-1))						// Continue until no newline is found or last line to write is reached
				{
					bWriteLine=0;														// Reset bWriteLine
					if(fromline!=-1)													// Is fromline specified?
					{
						if(toline!=-1)													// Is toline specified?
						{
							if(curline>=fromline&&curline<=toline)bWriteLine=1;			// If so, only write lines from <fromline> until <toline>
						}
						else if(curline>=fromline)bWriteLine=1;							// else, only write lines from <fromline>
					}
					else bWriteLine=1;
					if(bWriteLine)
					{
#ifdef _UNICODE
						dwLen = (wcslen(prefix)) * sizeof(wchar_t);
						WriteFile ( hFile, (LPVOID) prefix, dwLen, &nBytesDone, NULL);
						dwLen = (wcslen(token) + 1) * sizeof(wchar_t);
						WriteFile ( hFile, (LPVOID) token, dwLen, &nBytesDone, NULL);
						WriteFile(hFile,_T("\r\n"),4,&nBytesDone,NULL);						// Write Newline to logfile
#else
						WriteFile(hFile,prefix,strlen(prefix),&nBytesDone,NULL);		// Write prefix to logfile
						WriteFile(hFile,token,strlen(token),&nBytesDone,NULL);			// Write current line to logfile
						WriteFile(hFile,_T("\r\n"),2,&nBytesDone,NULL);						// Write Newline to logfile
#endif
					}
					curline++;															// Next line
					token=_tcstok(NULL,_T("\r\n"));											// Find next Newline
				}
				VirtualFree(readbuf,0,MEM_RELEASE);										// Release memory for filebuffer
			}
			CloseHandle(hAddFile);														// Release File
		}
	}
}

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    g_hInstance = hInst;
	return TRUE;
}

void WriteLine(TCHAR* pbuffer)
{
	int nBytesDone=0;
#ifdef _UNICODE
	DWORD dwLen;
	dwLen = (wcslen(pbuffer)) * sizeof(wchar_t);
	WriteFile ( hFile, (LPVOID) pbuffer, dwLen, &nBytesDone, NULL);
	WriteFile(hFile,_T("\r\n"),4,&nBytesDone,NULL);
#else
	WriteFile(hFile,pbuffer,strlen(pbuffer),&nBytesDone,NULL);
	WriteFile(hFile,_T("\r\n"),2,&nBytesDone,NULL);
#endif
}

