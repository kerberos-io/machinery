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
    // Locate kerberos working directory

    std::string workDirectory = helper::getRootDirectory(argv[0]);
    
    while(true)
    {
        try
        {
            // ---------------------------------------------
            // Bootstrap kerberos with a configuration file.

            std::string configFile = (argc > 1) ? argv[1] : "/etc/kerberosio/config/config.xml";
            Kerberos::run(configFile);
        }
        catch(Exception & ex)
        {
            // ------------------------------------------
            // Exceptions are logged in "log.stash" file.
        
            std::ofstream logstash;
            std::string logFile = (argc > 2) ? argv[2] : "/etc/kerberosio/logs/log.stash";
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
