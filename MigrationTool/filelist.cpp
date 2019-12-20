// FileList - List of files by template

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/io.h>
#include <unistd.h>

#define _read read
#define _write write
#define _close close

#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include "filelist.h"
#include "file.h"
#include "str.h"

// Constructor
FileList::FileList()
{
	_size = 0;
}

// Load the list of files
int FileList::Load(const char *inputPath)
{
	if(inputPath == NULL)
		return -1;

	const char *cur = inputPath;

	// Iterate through all comma separated items specified in the path
	while(*cur)
	{
		std::string item;

		// Get next item from the list
		cur = Str::GetNextInList(cur, item);

		if(item.empty())
			return 0;

		// Check whether the item points to an existing directory
		bool isDir = File::IsDirectory(item.c_str());

		std::string dir;
		std::string file;

		// If item is a directory, add *.* for files
		if(isDir)
		{
			dir = item;
			file = "*.*";
		}
		else
		{
			// Split directory and file parts
			File::SplitDirectoryAndFile(item.c_str(), dir, file);
		}

		std::list<std::string> dirs;

		// Directory may contain a wildcard, extend it
		GetDirectoriesByWildcard(dir.c_str(), dirs);

		// Find files, the directory list may grow(!) as new sub-directories can be found
		for(std::list<std::string>::iterator i = dirs.begin(); i != dirs.end(); i++)
		{
			// Find files in the specified directory matching the file wildcard
			FindFiles(*i, file, dirs, _files); 
		}

	}

	return 0;
}

// Get the directory list matching the specified wildcard (A*\B*\C\D* i.e.)
int FileList::GetDirectoriesByWildcard(const char *wildcard, std::list<std::string> &dirs)
{
	if(wildcard == NULL)
		return -1;

	// If wildcard is not specified add to the list and return
	if(strchr(wildcard, '*') == NULL)
	{
		std::string dir = wildcard;
		dirs.push_back(dir);

		return 0;
	}

	return 0;
}

// Find files in the specified directory matching the file wildcard
int	FileList::FindFiles(std::string dir, std::string file, std::list<std::string> &dirs, std::list<std::string> &files)
{
	std::string dir_sep = (dir.empty() == false) ? dir + DIR_SEPARATOR_STR : "";

	// Get the current search path
	std::string path =  dir_sep + file;

	// If the file does not contain a wildcard add as is
	if(strchr(file.c_str(), '*') == NULL)
	{
		size_t size = 0;

		if(File::IsFile(path.c_str(), &size) == true)
		{
			files.push_back(path);
			_size += size;

			return 0;
		}
	}

#ifdef WIN32

	struct _finddata_t fileInfo;

	int searchHandle = _findfirst(path.c_str(), &fileInfo); 
	if(searchHandle == -1)
		return 0;

	do
	{
		// If a file is found, add it to the list
		if(File::IsFile(&fileInfo))
		{
			std::string foundFile = dir_sep;
			foundFile += fileInfo.name;

			files.push_back(foundFile);

			// Total size in bytes
			_size += fileInfo.size;
		}
		else
		// if a directory found (skipping "." and "..") and to the directory list for further processing
		if(File::IsDirectory(&fileInfo))
		{
			std::string foundDir = dir_sep;
			foundDir += fileInfo.name;

			dirs.push_back(foundDir);
		}

	} while(_findnext(searchHandle, &fileInfo) == 0); 

	_findclose(searchHandle);

#else

	// Get the name of temporary file to store directory listing
	char temp[256]; *temp = '\x0';
	sprintf(temp, "sl_ls_%d.txt", getpid());

	std::string command = "ls -1 -d ";
	command += path;
	command += " 2>/dev/null > ";
	command += temp;

	// Read matched files
	if(system(command.c_str()) != -1)
	{
		FILE *file = fopen(temp, "r");
		char fileName[1024]; *fileName = '\x0';
  
		if(file)
		{
			// Read file content line by line
			while(fgets(fileName, 1024, file))
			{
				// Remove new line from path, otherwise stat() will fail
				int len = strlen(fileName);
				if(len && fileName[len-1] == '\n')
					fileName[len-1] = '\x0'; 

				size_t size = 0;
				bool file = File::IsFile(fileName, &size);

				if(file == true)
				{
					files.push_back(fileName);
					_size += size;
				}
			 }
		}
	 
		fclose(file);
   }

   // delete the temporary file
   unlink(temp);

#endif

	return 0;
}
