// OS Class - Operating system related functions

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdio.h>
#include "os.h"

// Get procedure address
void* Os::GetProcAddress(int module, const char *name)
{
#ifdef WIN32
	FARPROC proc = ::GetProcAddress((HMODULE)module, name);

	return proc;
#else
    return NULL;
#endif
}

// Get current time in milliseconds
int Os::GetTickCount()
{
#ifdef WIN32
	return ::GetTickCount();
#else
	struct timeval  tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec * 1000 + tv.tv_usec/1000;
#endif
}

// Get error message of the last system error
void Os::GetLastErrorText(const char *prefix, char *output, int len)
{
	if(output == NULL || len <= 0)
		return;

#ifdef WIN32
   	char error[1024];

    DWORD rc = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
						1033, (LPSTR)error, 1024, NULL);
 
	// No error found
	if(rc == 0)
	{
		*output = '\x0';
		return;
	}

	strcpy(output, prefix);
	strcat(output, error);

#endif
}
