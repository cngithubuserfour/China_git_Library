// MigrationTool Class

#ifdef WIN32
#include <windows.h>
#include <conio.h>
#endif

#include <stdio.h>
#include <string.h>
#include "MigrationTool.h"
#include "filelist.h"
#include "file.h"
#include "sqlparserexp.h"
#include "os.h"
#include "str.h"
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Constructor/destructor
MigrationTool::MigrationTool()
{
	_parser = CreateParserObject();
	_total_files = 0;
    
	_exe = NULL;
}

// Run the tool with command line parameters
int MigrationTool::Run(int argc, char** argv)
{
	_total_files = 0;

	// Read and validate parameters
	int rc = SetParameters(argc, argv);

	// Command line or parameters error
	if(rc == -1)
		return rc;

    rc = ProcessFiles();

	return rc;
}

// Perform operations on files
int MigrationTool::ProcessFiles()
{
	FileList fileList;

	PrintCurrentTimestamp();

	// Load the list of files
    int rc = fileList.Load(_srcfile.c_str());

	char total_size_fmt[21];
	Str::FormatByteSize(fileList.GetSize(), total_size_fmt);

	_total_files = fileList.Get().size();

	_log.Log("\n\nSource file%s: %d (%s)\n", SUFFIX(_total_files), _total_files, total_size_fmt);

	int num = 1;
	int total_lines = 0;

	int all_start = Os::GetTickCount();

	// Handle each file
//	for(std::list<std::string>::iterator i = fileList.Get().begin(); i != fileList.Get().end(); i++, num++)
//	{
		int start = Os::GetTickCount();

        std::string current = _srcfile;
		std::string relative_name = File::GetRelativeName(_in.c_str(), current.c_str()); 

		_log.Log("\n%5d. %s", num, relative_name.c_str()); 

        std::string out_name = _dstfile/*GetOutFileName(*i, relative_name)*/;

		int in_size = 0;
		int in_lines = 0;

        SetParserOption(_parser, MIGRATION_CURRENT_FILE, relative_name.c_str());

		// Convert the current file
	    rc = ProcessFile(current, out_name, &in_size, &in_lines);

		total_lines += in_lines;

		int end = Os::GetTickCount() - start;

		char time_fmt[21];
		char size_fmt[21];

		Str::FormatTime(end, time_fmt);
		Str::FormatByteSize(in_size, size_fmt);

		_log.Log("...Ok (%s, %d line%s, %s)", size_fmt, in_lines, SUFFIX(in_lines), time_fmt); 
//	}

	char total_time_fmt[21];
	Str::FormatTime(Os::GetTickCount() - all_start, total_time_fmt);

    if(_total_files > 0)
    {
        char summary[1024];
        sprintf(summary, "\n\nTotal: %d file%s, %s, %d line%s, %s", _total_files, SUFFIX(_total_files), 
            total_size_fmt, total_lines, SUFFIX(total_lines), total_time_fmt);

		_log.Log("%s", summary);

        if(_a)
        {
            _log.Log("\n\nCreating assessment report");
            CreateAssessmentReport(_parser, summary);
        }
    }

	PrintCurrentTimestamp();

	return rc;
}

// Get output name of the file
std::string MigrationTool::GetOutFileName(std::string &input, std::string &relative_name)
{
	std::string output;

	// If -out option not set, add "_out" to the file name
	if(_out.empty() == true)
	{
		std::string dir;
		std::string file;

		// Get the input directory and file name
		File::SplitDirectoryAndFile(input.c_str(), dir, file);

		if(dir.empty() == false)
		{
			output = dir;
			output += DIR_SEPARATOR_CHAR;
		}

		// When there is no extension pos points to '\x0'
		int pos = File::GetExtensionPosition(file.c_str());

		std::string out_file;

		if(pos > 0)
			out_file.assign(file, 0, pos);

		// Add default postfix
		out_file += "_out";

		// Extension exists (.at(pos) throws an exception when pos point to 0)
		if(pos > 0 && (file.c_str())[pos] != '\x0')
			out_file.append(file.c_str() + pos);

		output += out_file;
	}
	// -out option is set
	else
	{
		int len = _out.size();

		// If multiple files are converted, -out must specify the directory; also if path terminates with \ the directory is specified
		if(_total_files > 1 || (len > 0 && _out[len-1] == DIR_SEPARATOR_CHAR))
		{
			// Get file name (relative name can include a subdirectory)
			File::GetPathFromDirectoryAndFile(output, _out.c_str(), relative_name.c_str());

			std::string dir;
			std::string file;

			File::SplitDirectoryAndFile(output.c_str(), dir, file);

			// Create target directories
			File::CreateDirectories(dir.c_str());
		}
		// If single file is converted, -out must specify the file name (with or without directory)
		else
			output = _out;
	}

	return output;
}

// Process a file
int MigrationTool::ProcessFile(std::string &file, std::string &out_file, int *in_size, int *in_lines)
{
	if(_parser == NULL)
		return -1;

	int size = File::GetFileSize(file.c_str());

	if(size <= 0)
		return -1;

	// Allocate a buffer for the file content
	char *input = new char[size];

	// Get content of the file (without terminating 'x0')
	int rc = File::GetContent(file.c_str(), input, size);

	if(rc == -1)
	{
		delete input;
		return -1;
	}

	const char *output = NULL;
	int out_size = 0;
	int lines;

	// Convert the file
    rc = ConvertSql(_parser, input, size, &output, &out_size, &lines);

    // Write the target content to the file
    rc = File::Write(out_file.c_str(), output, out_size);

    FreeOutput(output);

	if(in_size != NULL)
		*in_size = size;

	if(in_lines != NULL)
		*in_lines = lines;

	delete input;

	return rc;
}

// Read and validate parameters
int MigrationTool::SetParameters(int argc, char **argv)
{
	if(argv != NULL)
		_exe = argv[0];

	// Fill parameters map
	int rc = _parameters.Load(argc, argv);

	// Get -log option
    char *value = _parameters.Get(LOG_OPTION);

	if(value != NULL)
		_logfile = value;
	else
        _logfile = MIGRATION_LOGFILE;

	// Set the directory and name of log file
	_log.SetLogfile(_logfile.c_str());

    // Get -p option
    value = _parameters.Get(PARAM_OPTION);

    if(value != NULL)
        _in = value;

    std::ifstream pf(_in);
    if (!pf.is_open())
        return -1;

    json param;
    pf >> param;

    _srcfile = param["SourceDB"]["FilePath"].get<std::string>();
    _dstfile = param["DestinationDB"]["FilePath"].get<std::string>();

    _parameters.GetMap().insert(ParametersPair("-s", _srcfile));
    _parameters.GetMap().insert(ParametersPair("-d", _dstfile));

    pf.close();

	// Get -out option
	value = _parameters.Get(OUT_OPTION);

	if(value != NULL)
		_out = value;

    // Get -a option
	value = _parameters.Get(A_OPTION);

	if(value != NULL)
		_a = true;

	if(_parameters.Get(HELP_PARAMETER))
	{
		PrintHowToUse();
		return -1;
	}

	SetTypes();
	SetOptions();

	return rc;
}

// Set source and target types
void MigrationTool::SetTypes()
{
    int source = SQL_ORACLE;
    int target = SQL_MYSQL;

	SetParserTypes(_parser, source, target);
}

// Set conversion options
void MigrationTool::SetOptions()
{
	ParametersMap &map = _parameters.GetMap();

	for(ParametersMap::iterator i = map.begin(); i != map.end(); ++i)
		SetParserOption(_parser, i->first.c_str(), i->second.c_str());
}

// Define SQL dialect type by name
short MigrationTool::DefineType(const char *name)
{
	if(name == NULL)
		return -1;

	short type = 0;

	if(_stricmp(name, "oracle") == 0)
		type = SQL_ORACLE;
	else
	if(_stricmp(name, "mysql") == 0)
		type = SQL_MYSQL;
	
	return type;
}

// Output how to use the tool if /? or incorrect parameters are specified
void MigrationTool::PrintHowToUse()
{
	printf("\n\nHow to use:");
    printf("\n\n    migrationtool -option=value [...n]");

	printf("\n\nOptions:\n");
    printf("\n   -p        - Parameter file");
	printf("\n   -out      - Output directory (the current directory by default)");
	printf("\n   -log      - Log file (sqlines.log by default)");
	printf("\n   -?        - Print how to use");

	printf("\n\nExample:");
#ifdef WIN32
    printf("\n\nConvert script.sql file from Oracle to MySQL");
    printf("\n\n   MigrationTool.exe -p=param.json ");

	printf("\nPress any key to continue...\n");

	_getch();
#else
	printf("\n\nConvert script.sql file from Oracle to MySQL");
    printf("\n\n   ./MigrationTool -p=param.json");
	printf("\n");
#endif
	printf("\n");
}

// Output the current date and time
void MigrationTool::PrintCurrentTimestamp()
{
#ifdef WIN32
	SYSTEMTIME lt;
	GetLocalTime(&lt);

	// Write log record
	_log.LogFile("\n\nCurrent timestamp: %d-%02d-%02d %02d:%02d:%02d.%03d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond,
		lt.wMilliseconds);
#else
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);

	_log.LogFile("\n\nCurrent timestamp: %d-%02d-%02d %02d:%02d:%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
#endif
}
