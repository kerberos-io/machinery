//
//  Class: Watcher
//  Description: File watcher, will check if news files has been
//               added to a specific directory, and create a symbolic link.
//  Created:     05/02/2016
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#include "Factory.h"
#include "Guard.h"
#include <fstream>

#ifndef __Watcher_H_INCLUDED__   // if Watcher.h hasn't been included yet...
#define __Watcher_H_INCLUDED__   // #define this so the compiler knows it has been included

namespace kerberos
{
    class Watcher
    {
        private:
            std::string m_fileDirectory;
            FW::Guard * guard;
            pthread_mutex_t * m_cloudLock;
        
        public:
            Watcher(){}
            ~Watcher(){delete guard;}
            void setup(const char * fileDirectory);
            void setLock(pthread_mutex_t * lock)
            {
                m_cloudLock = lock;
            }
        
            static void addFile(const std::string & file);
            void scan();
    };
}
#endif