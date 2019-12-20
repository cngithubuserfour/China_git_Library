// Str - String operations 

#ifndef sqlines_str_h
#define sqlines_str_h

#if !defined(WIN32) && !defined(WIN64)

#define _stricmp strcasecmp
#define _strnicmp strncasecmp

char *_strupr(char *input);
char *_strlwr(char *input);
#endif

#include <string>

class Str
{
public:
	// Skip spaces, new lines and tabs
	static char* SkipSpaces(const char *input);
	// Skip comments, spaces, new lines and tabs
	static char *SkipComments(const char *input);
	
	// Trim trailing spaces
	static void TrimTrailingSpaces(std::string &input);
	static char* TrimTrailingSpaces(char *input);

	// Skip until the specified char is met
	static const char* SkipUntil(const char *input, char ch);

	// Get substring until the specified char is met
	static void GetSubtringUntil(const char *input, char ch, int len, std::string &output);

	// Get copy of the input string
	static char* GetCopy(const char *input, size_t size);
	static char* GetCopy(const char *input);

	// Replace character in string (returns the same string)
	static char* ReplaceChar(char *input, const char what, const char with);

	// Convert int to string
	static char* IntToString(int int_value, char *output);

	// Replace the first occurrence of a substring
	static void ReplaceFirst(std::string &str, std::string what, std::string with);

	// Get next item in list
	static char* GetNextInList(const char *input, std::string &output);

	// Convert size in bytes to string with MB, GB
	static char* FormatByteSize(double bytes, char *output);

	// Convert time in milliseconds to string with ms, sec
	static char* FormatTime(int time_ms, char *output);

	// Convert 2 digit date time part (century, 2 digit year, month, day, hour, minute, second) to 2 chars
	static void Dt2Ch(int dt, char *ch);

	// Does character points to space, tab or new line
	static bool IsSpace(char c) { if(c == ' ' || c == '\t' || c == '\r' || c == '\n') return true; return false; }
};

#endif // sqlines_str_h
