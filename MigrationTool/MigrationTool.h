#ifndef migrationtool_migrationtool_h
#define migrationtool_migrationtool_h

#include <string>
#include "applog.h"
#include "parameters.h"

#define PARAM_OPTION                "-p"        // Parameter file
#define OUT_OPTION                  "-out"      // Output directory
#define A_OPTION                    "-a"        // Assessment
#define LOG_OPTION                  "-log"      // Log file

#define MIGRATION_CURRENT_FILE        "__cur_file__"   // Relative path for the current file

// Default log file name
#define MIGRATION_LOGFILE             "migration.log"

#define SUFFIX(int_value)           ((int_value == 1) ? "" : "s")

class MigrationTool
{
    // Options
    std::string _in;
    std::string _out;
	std::string _logfile;
    std::string _srcfile;
    std::string _dstfile;

    bool _a;

    // Current executable file
    const char *_exe;

    // SQL parser handle 
    void *_parser;

    // Total number of files
    int _total_files;

    // Command line parameters
    Parameters _parameters;
    // Logger
    AppLog _log;

public:
    MigrationTool();

    // Run the tool with command line parameters
    int Run(int argc, char** argv);

private:
    // Read and validate parameters
    int SetParameters(int argc, char **argv);

    int ProcessFiles();
    int ProcessFile(std::string &file, std::string &out_file, int *in_size, int *in_lines);

    // Get output name of the file
    std::string GetOutFileName(std::string &input, std::string &relative_name);

    // Set source and target types
    void SetTypes();
    short DefineType(const char *name);

    // Set conversion options
    void SetOptions();

    // Output how to use the tool if /? or incorrect parameters are specified
    void PrintHowToUse();
    // Output the current date and time
    void PrintCurrentTimestamp();
};

#endif // migrationtool_sqlparser_h
