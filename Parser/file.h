// File - File operations

#ifndef sqlines_file_h
#define sqlines_file_h

#include <string>

#ifdef WIN32
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_STR  "\\"

struct _finddata_t;

#else
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR  "/"
#endif

class File
{
public:
	// Check if the path points to an existing directory
	static bool IsDirectory(const char *path);	
	// Check if the path points to an existing file
	static bool IsFile(const char *path, size_t *size);	

#ifdef WIN32
	// Check if the file is directory
	static bool IsDirectory(_finddata_t *fileInfo);
	// Check if the file is found
	static bool IsFile(_finddata_t *fileInfo);
#endif

	// Split directory and file parts from a path
	static void SplitDirectoryAndFile(const char* path, std::string &dir, std::string &file);

	// Get last directory separator position
	static int GetLastDirSeparatorPos(const char *path);

	// Get the position of file extension
	static int GetExtensionPosition(const char *path);

	// Get the size of the file
	static int GetFileSize(const char* file);

	// Get content of the file (without terminating with 'x0')
	static int GetContent(const char* file, void *input, size_t len);

	// Get relative name
	static std::string GetRelativeName(const char* base, const char *file);

	// Write the buffer to the file
    static int Write(const char *file, const char* content, size_t size);
};

#endif // sqlines_file_h
