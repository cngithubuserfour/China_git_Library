#include <iostream>
#include <fstream>
#include "MigrationTool.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

constexpr auto PARAM_FILE = "param.json";

int main(int argc, char** argv)
{
    int rc = 0;

    MigrationTool migration;

    try
    {
        // Run MigrationTool in command line
        rc = migration.Run(argc, argv);
    }
    catch(...)
    {
        return -2;
    }

    printf("\n");

    return rc;
}
