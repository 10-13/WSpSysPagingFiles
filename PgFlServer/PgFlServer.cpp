// PgFlServer.cpp : Defines the entry point for the console application.
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
		
	hEventTermination = CreateEvent(NULL,FALSE,//auto-reset
		FALSE,//nonsignaled
		szEventTerminationName);
	if(!hEventTermination) {
		printf("Create Event <%s>: Error %ld\n",szEventTerminationName,GetLastError());
		printf("Press any key to quit...\n");
		getch();		return 0;
	}
	//??Here is something wrong!!!
	if(ERROR_ALREADY_EXISTS==GetLastError()) {
		printf("PgFlServer has already started\n");
		printf("Press any key to quit...\n");
		getch();		return 0;
	}
	
	printf("PgFlServer starting ...\n");

	hEventChar = CreateEvent(NULL,FALSE,FALSE,szEventCharName);//auto-reset,nonsignaled
	if(!hEventChar)	{
		printf("Create Event <%s>: Error %ld\n",szEventCharName,GetLastError());
		printf("Press any key to quit...\n");
		getch();		return 0;
	}
	hPagingFileMapping = CreateFileMapping((HANDLE)-1,//== INVALID_HANDLE_VALUE
											NULL,
											PAGE_READWRITE,
											0,200,szPagingFileShareName);
	//that is backed by the system paging file instead of by a file in the file system.
	if(!hPagingFileMapping)	{
		printf("Create File Mapping <%s>: Error %ld\n",szPagingFileShareName,GetLastError());
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

	printf("PgFlServer is ready to receive input from PgFlClient...\n");
//===================================================================//
	HANDLE hEvents[]={hEventTermination,hEventChar};
	bool bTerminate=false;
	DWORD dwWaitRetCode;

	while(!bTerminate)
	{
		dwWaitRetCode = WaitForMultipleObjects(2,hEvents,
									FALSE,//If FALSE, the function returns 
									//when the state of any one of the objects set to is signaled
									INFINITE);
		switch(dwWaitRetCode  )
		{					
		case WAIT_FAILED : 
			bTerminate=true;
			printf("PgFlServer terminated by WAIT_FAILED: Error %ld\n",GetLastError());
			continue;

		case WAIT_OBJECT_0  :    //either hEvents[0] or both hEvents[0] and hEvents[1]
			bTerminate=true;
			printf("PgFlServer terminated by hEventTermination: GetLastError()= %ld\n",GetLastError());
			continue;
			 
		case WAIT_OBJECT_0 + 1 : //only one hEvents[1] is signaled

			putch(*((LPSTR)lpFileMap));
			continue;
			
		default ://if it is possible
			bTerminate=true;
			printf("PgFlServer terminated <default>: GetLastError() %ld\n",GetLastError());
			continue;
		}//switch
	}//while
//-----------------------------------//
	CloseHandle(hEventTermination);
	CloseHandle(hEventChar);

	UnmapViewOfFile(lpFileMap);
	CloseHandle(hPagingFileMapping);



	printf("Hello World!\n");
	printf("PgFlServer closed...\n");
	printf("Press any key to quit...\n");
	getch();
	return 0;
}

