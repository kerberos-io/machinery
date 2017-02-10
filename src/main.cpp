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

_INITIALIZE_EASYLOGGINGPP

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
    
    // ----------------------------------
    // Get parameters from command line
    
    StringMap parameters = helper::getCommandOptions(argc, argv);
    
    // --------------------------z--------
    // Initialize logger
                  
    easyloggingpp::Configurations config;
    config.setToDefault();
    config.setAll(easyloggingpp::ConfigurationType::Enabled, "true");
    config.setAll(easyloggingpp::ConfigurationType::ToFile, "true");
    std::string logFile = (helper::getValueByKey(parameters, "log")) ?: LOG_PATH;
    config.setAll(easyloggingpp::ConfigurationType::Filename, logFile);
    config.setAll(easyloggingpp::ConfigurationType::RollOutSize, "100000"); // 100MB
    easyloggingpp::Loggers::reconfigureAllLoggers(config);

    LINFO << "Logging is written to: " + logFile;
    
    while(true)
    {
        try
        {
            // ----------------------------------
            // Bootstrap machinery with parameters

            Kerberos::run(parameters);
            
        }
        catch(Exception & ex)
        {
            LERROR << ex.what();
            
            // Try again in 3 seconds..
            usleep(3 * 1000000);
        }
    }

    return 0;
}