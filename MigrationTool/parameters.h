// Parameters class - Loads parameters from command line 

#ifndef migrationtool_parameters_h
#define migrationtool_parameters_h

#include <map>

#define PARAMETER_START_TOKEN	'-'				

// Option to print help on available parameters (? is also suported)
#define HELP_PARAMETER			"-?"				

typedef std::map<std::string, std::string> ParametersMap;
typedef std::pair<std::string, std::string> ParametersPair;

//////////////////////////////////////////////////////////////////////

class Parameters
{
	// Key-value map of parameters
	ParametersMap _map;

public:
	// Load parameters from command line and configuration file
	int Load(int argc, char** argv);

	// Get the value by key
	char* Get(const char *key);

	ParametersMap& GetMap() { return _map; }

private:
	// Load command line parameters
	int LoadCommandLine(int argc, char** argv);
	// Load parameters from a string
	int LoadString(const char *input);
};

#endif // migrationtool_parameters_h
