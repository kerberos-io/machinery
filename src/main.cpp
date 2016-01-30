//
//  Project: Kerberos.io
//  Description: Open source video surveillance
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied.
//
/////////////////////////////////////////////////////

#include "Kerberos.h"
#include "Helper.h"
#include <iostream>
#include <fstream>

using namespace kerberos;

int main(int argc, char** argv)
{
    // ----------------------------------
    // Get parameters from command line
    
    StringMap parameters = helper::getCommandOptions(argc, argv);
    
    while(true)
    {
        try
        {
            // ----------------------------------
            // Bootstrap kerberos with parameters
            
            Kerberos::run(parameters);
        }
        catch(Exception & ex)
        {
            // ------------------------------------------
            // Exceptions are logged in "log.stash" file.
            
            std::ofstream logstash;
            std::string logFile = (helper::getValueByKey(parameters, "log")) ?: LOG_PATH;
            logstash.open(logFile.c_str(), std::ios_base::app);
            
            if(logstash.is_open())
            {
                const char * error = ex.what();
                logstash << helper::currentDateTime() << " - " << error << std::endl;
                logstash.close();
                delete (char *) error;
            }
        
            // Try again in 10 seconds..
            usleep(10 * 1000000);
        }
    }

    return 0;
}