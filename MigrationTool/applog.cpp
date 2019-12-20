// AppLog - Application logger class

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "applog.h"

// Constructor
AppLog::AppLog()
{
	_filename = APPLOG_DEFAULT_FILE;
	_trace = false;
}

// Log message to console and log file
void AppLog::Log(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	// log message to console
	vprintf(format, args);

	// QT tools seem to get delayed output
	fflush(stdout);

        va_end(args);
        va_start(args, format);

	// log message to file
	LogFileVaList(format, args);

	va_end(args);
}

// Log messages to file if trace mode is set
void AppLog::Trace(const char *format, ...)
{
	if(_trace == false)
		return;

	va_list args;
	va_start(args, format);

	// log message into the file
	LogFileVaList(format, args);

	va_end(args);
}

// Write to console only
void AppLog::LogConsole(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	// log message to console
	vprintf(format, args);

	fflush(stdout);

	va_end(args);
}

// Write to log file only
void AppLog::LogFile(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	// log message into the file
	LogFileVaList(format, args);

	va_end(args);
}

// Write to log file
void AppLog::LogFileVaList(const char *format, va_list args)
{
	static bool firstCall = true;
	const char *openMode = "a";

	// During the first call destroy the log file content that may exists from the previous runs
	if(firstCall)
		openMode = "w";

	FILE *file = fopen(_filename.c_str(), openMode);

	// Log message to file
	if(file != NULL)
	{
		vfprintf(file, format, args);
		fclose(file);
	}
	else 
	{  
		// Show error message during the first call only  
		if(firstCall)
			printf("\n\nError:\n Opening log file %s - %s", _filename.c_str(), strerror(errno));
	} 

	firstCall = false;
}
