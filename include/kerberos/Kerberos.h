//
//  Class: Kerberos
//	Description: Entry point of application.
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//	Website:	 www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#ifndef __Kerberos_H_INCLUDED__   // if Kerberos.h hasn't been included yet...
#define __Kerberos_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Factory.h"
#include "machinery/Machinery.h"
#include "Guard.h"
#include "document.h" // rapidjson

#define CONFIGURATION_PATH "/etc/kerberosio/config/config.xml"
#define LOG_PATH "/etc/kerberosio/logs/log.stash"

namespace kerberos
{
    class Kerberos
    {
        private:
            FW::Guard * guard;
            Capture * capture;
            Machinery * machinery;
            ImageVector images;
            int m_captureDelayTime;
            StringMap m_parameters;

            Kerberos(){};
            ~Kerberos(){delete guard; delete capture; delete machinery;};

            void bootstrap(StringMap & parameters);
            void configure(const std::string & configuration);
            void setCaptureDelayTime(int delay){m_captureDelayTime=delay;};
            void setParameters(StringMap & parameters){m_parameters = parameters;};
            StringMap getParameters(){return m_parameters;}
        
        public:
            // -----------
            // Singleton

            static Kerberos * getInstance()
            {
                static Kerberos _kerberos;
                return &_kerberos;
            }

            // ----------------
            // Run application

            static void run(StringMap & parameters)
            {
                getInstance()->bootstrap(parameters);
            }

            // -----------------------------------
            // Alias for configure, more readable
            
            static void reconfigure(const std::string & configuration)
            {
                getInstance()->configure(configuration);
            }
    };
}
#endif
