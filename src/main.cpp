//
//  Project: Kerberos.io
//  Description: Open source video surveillance
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        cedric@verstraeten.io
//  Website:     www.verstraeten.io
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied under
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#include "Kerberos.h"
#include "Helper.h"
#include <iostream>
#include <fstream>
#include <signal.h>
#include "easylogging++.h"
#define ELPP_THREAD_SAFE
INITIALIZE_EASYLOGGINGPP

using namespace kerberos;

int main(int argc, char** argv)
{
    // ---------------------
    // If requesting version

    if(argc == 2 && strcmp(argv[1],"-v")==0)
    {
        std::cout << "v" << VERSION << std::endl;
        return 1;
    }

    // ----------------
    // Disable SIGPIPE

    // # old way  -> signal(SIGPIPE, SIG_IGN);
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigaction(SIGPIPE, &sa, 0);

    // --------------------------------
    // Get parameters from command line

    StringMap parameters = helper::getCommandOptions(argc, argv);

    // -----------------
    // Initialize logger

    el::Configurations config;
    config.setToDefault();
    config.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
    config.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
    config.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
    config.set(el::Level::Global, el::ConfigurationType::LogFlushThreshold, "100");
    config.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, "5000000"); // 5MB
    std::string logFile = (helper::getValueByKey(parameters, "log")) ?: LOG_PATH;
    config.set(el::Level::Global, el::ConfigurationType::Filename, logFile.c_str());
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::reconfigureAllLoggers(config);

    VLOG(0) << "Logging is written to: " + logFile;

    while(true)
    {
        try
        {
            // -----------------------------------
            // Bootstrap machinery with parameters

            Kerberos::run(parameters);

        }
        catch(Exception & ex)
        {
            LOG(ERROR) << ex.what();

            // Try again in 3 seconds..
            usleep(3 * 1000000);
        }
    }

    return 0;
}
