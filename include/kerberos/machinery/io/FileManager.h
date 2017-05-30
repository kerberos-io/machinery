//
//  Class:        FileManager
//  Description: A filemanager for creating directories, files, etc.
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

#ifndef __FileManager_H_INCLUDED__   // if FileManager.h hasn't been included yet...
#define __FileManager_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Factory.h"
#include "capture/Image.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

namespace kerberos
{
    class FileManager
    {
        private:
            std::string m_baseDir;
            
        public:
            FileManager(){};
            FileManager(char * baseDir):m_baseDir(baseDir){};
            FileManager(std::string & baseDir):m_baseDir(baseDir){};
            ~FileManager(){};
		
            void setBaseDirectory(std::string baseDir){m_baseDir = baseDir;}
            std::string & getBaseDirectory(){return m_baseDir;}
            bool createDirectoryIfNotExists(std::string & dir);
            bool createDirectoryIfNotExists(char * dir);
            bool createDirectory(std::string & path);
            bool createDirectory(const char * path);
            bool save(Image & image, const std::string & path, bool createSymbol);
    };
}
#endif 
