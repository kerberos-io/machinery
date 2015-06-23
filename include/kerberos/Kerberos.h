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

            Kerberos(){};
            ~Kerberos(){delete guard; delete capture; delete machinery;};

            void bootstrap(const std::string & configuration);
            void configure(const std::string & configuration);
            void setCaptureDelayTime(int delay){m_captureDelayTime=delay;};
        
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

            static void run(const std::string & configuration)
            {
                getInstance()->bootstrap(configuration);
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
