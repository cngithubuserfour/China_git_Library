#ifndef migrationtool_applog_h
#define migrationtool_applog_h

#include <stdarg.h>
#include <string>

// Default log file name
#define APPLOG_DEFAULT_FILE			"applog.log"

class AppLog
{
	// Log file name
	std::string _filename;
	// Trace mode 
	bool _trace;
public:
	AppLog();

	// Log message to console and log file
    void Log(const char *format, ...);
    // Log messages to file if trace mode is set
    void Trace(const char *format, ...);
    // Write to console only
    void LogConsole(const char *format, ...);
    // Write to log file only
    void LogFile(const char *format, ...);

	// Set log file name
	void SetLogfile(const char *name) { _filename = name; }
private:
    // Write to log file
    void LogFileVaList(const char *format, va_list args);	
};

#endif // migrationtool_applog_h
