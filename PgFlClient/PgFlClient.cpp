// PgFlClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include "conio.h"

char szPagingFileShareName[] ="{11FB95B0-4300-49fb-BE12-B086FD00D7B8}";//"$$UniquePagingFileShareName$$";
//"{11FB95B0-4300-49fb-BE12-B086FD00D7B8}"
char szEventCharName[]		 ="{D244D5E4-4640-4186-BCC2-701BDE8E26DC}";//"$$UniqueEventCharName$$";
//"{D244D5E4-4640-4186-BCC2-701BDE8E26DC}"
char szEventTerminationName[]="{F3358C89-E4AD-43f4-8D20-38A038F47459}";//"$$UniqueEventTerminationName$$";
//"{F3358C89-E4AD-43f4-8D20-38A038F47459}"
int main(int argc, char* argv[])
{
	

HANDLE	hEventChar,
		hEventTermination,
		hPagingFileMapping;
		
	hEventTermination = OpenEvent(EVENT_ALL_ACCESS,FALSE,//childs do not inherit this handle.
		szEventTerminationName);
	if(!hEventTermination) {
		printf("Open Event <%s>: Error %ld\n",szEventTerminationName,GetLastError());
		printf("Press any key to quit...\n");
		getch();		return 0;
	}
	//??Here is something wrong!!!

	if(ERROR_ALREADY_EXISTS==GetLastError()) {
		printf("PgFlServer has already started\n");
		printf("Press any key to continue...\n");
		getch();	
		////////////////////////return 0;
	}
	
///////////////////////////////	printf("PgFlServer starting ...\n");

	hEventChar = OpenEvent(EVENT_ALL_ACCESS,FALSE,szEventCharName);
	if(!hEventChar)	{
		printf("Open Event <%s>: Error %ld\n",szEventCharName,GetLastError());
		printf("Press any key to quit...\n");
		getch();		return 0;
	}
	hPagingFileMapping = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, //the handle cannot be inherited.
										szPagingFileShareName);
	if(!hPagingFileMapping)	{
		printf("Open File Mapping <%s>: Error %ld\n",szPagingFileShareName,GetLastError());
		printf("Press any key to quit...\n");
		getch();		return 0;
	}

LPVOID lpFileMap = MapViewOfFile(hPagingFileMapping,
								 FILE_MAP_READ|FILE_MAP_WRITE,
								 0,0,// high-order and low-order 32 bits of file offset
								 0);// number of bytes to map to the view;
//zero means the mapping extends from the specified offset to the end of the file mapping. 

	if(!lpFileMap)	{
		printf("Map View Of File <%s>: Error %ld\n",szPagingFileShareName,GetLastError());
		printf("Press any key to quit...\n");
		getch();		return 0;
	}
//===================================================================//
	printf("\nPgFlClient is ready to send input to PgFlServer...\n");
	printf("Type characters or press the key <ESC> to terminate ...\n");
	
	CHAR message[260];
	char szTitle[] = "lpFileMap";
		__try {
			*((int*)lpFileMap + 1020) = 8;

			MessageBox(0, "Writing 8 successful", szTitle, MB_OK);
		}
		
		__except (EXCEPTION_EXECUTE_HANDLER) {
			//TCHAR message[260];
			wsprintf(message, TEXT("EXCEPTION EXECUTE write Error: %ld"), GetLastError());
			MessageBox(0, message, szTitle, MB_OK | MB_ICONEXCLAMATION);


			SetEvent(hEventTermination);

			CloseHandle(hEventTermination);
			CloseHandle(hEventChar);

			UnmapViewOfFile(lpFileMap);
			CloseHandle(hPagingFileMapping);
			printf("Hello World!\n");
			printf("PgFlClient closed...\n");
			printf("Press any key to quit...\n");
			getch();
			return 0;
		}
	bool bTerminate=false;
	char achar;
	while(!bTerminate)
	{
		achar=getche();
		if(achar==27) {
			SetEvent(hEventTermination);
			bTerminate=true;
		}

		*((LPSTR)lpFileMap) = achar;
		SetEvent(hEventChar);		
	}
//-----------------------------------//
	CloseHandle(hEventTermination);
	CloseHandle(hEventChar);

	UnmapViewOfFile(lpFileMap);
	CloseHandle(hPagingFileMapping);



	printf("Hello World!\n");
	printf("PgFlClient closed...\n");
	printf("Press any key to quit...\n");
	getch();
	return 0;
}

