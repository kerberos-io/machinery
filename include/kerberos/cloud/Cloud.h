//
//  Class: Cloud
//  Description: Cloud storage services
//  Created:     05/02/2015
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//	Website:	 www.cedric.ws
//
//  The copyright to the computer program(s) herein
//  is the property of Cédric Verstraeten, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#include "Factory.h"

#ifndef __Cloud_H_INCLUDED__   // if Cloud.h hasn't been included yet...
#define __Cloud_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "cloud/Watcher.h"

namespace kerberos
{
    class Cloud
    {
        protected:
            const char * name;
            int m_min;
            int m_max;
            int m_interval;
        
        public:    
            pthread_t m_uploadThread;
            pthread_t m_watchThread;
        
            Cloud(){};
            virtual ~Cloud(){};
            virtual void setup(kerberos::StringMap & settings) = 0;
            virtual bool upload(std::string pathToImage) = 0;
            void scan();
        
            void startWatchThread(StringMap & settings);
            void stopWatchThread();
            void startUploadThread();
            void stopUploadThread();
    };

    template<const char * Alias, typename Class>
    class CloudCreator: public Cloud
    {
        protected:
            CloudCreator(){name = ID;}
            
        public:
            static Cloud* create(){return new Class();};
            static const char * ID;
    };
}
#endif
