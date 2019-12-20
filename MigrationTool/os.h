// OS Class - Operating system related functions

#ifndef migrationtool_os_h
#define migrationtool_os_h

class Os
{
public:
	// Get procedure address
	static void* GetProcAddress(int module, const char *name);

	// Get current time in milliseconds
	static int GetTickCount();

	// Get error message of the last system error
	static void GetLastErrorText(const char *prefix, char *output, int len);
};

#endif // migrationtool_os_h
