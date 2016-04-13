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
#include <sys/stat.h>
#include "Guard.h"

#ifndef __Cloud_H_INCLUDED__   // if Cloud.h hasn't been included yet...
#define __Cloud_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "restclient-cpp/restclient.h"

namespace kerberos
{
    class Cloud
    {
        protected:
            const char * name;
            int m_min;
            int m_max;
            int m_interval;
            FW::Guard * guard;
            std::string m_captureDirectory;
            pthread_mutex_t m_cloudLock;
        
        public:
            pthread_t m_pollThread;
            pthread_t m_uploadThread;
        
            Cloud(){};
            virtual ~Cloud(){};
            virtual void setup(kerberos::StringMap & settings) = 0;
            virtual bool upload(std::string pathToImage) = 0;
            void scan();
            void setLock(pthread_mutex_t & lock)
            {
                m_cloudLock = lock;
            }
        
            void startUploadThread();
            void stopUploadThread();
            void startPollThread();
            void stopPollThread();
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
