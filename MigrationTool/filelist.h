// FileList - List of files by template

#ifndef migrationtool_filelist_h
#define migrationtool_filelist_h

#include <string>
#include <list>

class FileList
{
	// List of files
	std::list<std::string> _files;
	// Total size of all files in the list in bytes
	int _size;  
public:
	FileList();

	// Load the list of files
	int Load(const char *inputPath);	

	// Get the directory list matching the specified wildcard (A*\B*\C\D* i.e.)
	int GetDirectoriesByWildcard(const char *wildcard, std::list<std::string> &dirs);

	// Get list
	std::list<std::string>& Get() { return _files; }
	// Get total size of all files in the list in bytes
	int GetSize() { return _size; }

private:
	// Find files in the specified directory matching the file wildcard
	int	FindFiles(std::string dir, std::string file, std::list<std::string> &dirs, std::list<std::string> &files); 
};

#endif // migrationtool_filelist_h
