//
//	Class: Helper
//	Description: Some simple helper functions
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

#ifndef __Helper_H_INCLUDED__   // if Helper.h hasn't been included yet...
#define __Helper_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Types.h"
#include "tinyxml.h"
#include "Exception.hpp"
#include <ctime>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <iomanip>
#include <dirent.h>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>
#include <sstream>
#include <cctype>

namespace kerberos
{
    namespace helper
    {
        void getFilesInDirectory(std::vector<std::string> &out, const std::string &directory);
        std::string urlencode(const std::string &value);
        const char * getValueByKey(kerberos::StringMap & map, const std::string & key);
        kerberos::StringMap getCommandOptions(int argc, char ** argv);
        kerberos::StringMap getSettingsFromXML(const std::string & path);
        std::string printStringMap(const std::string & prefix, const kerberos::StringMap & map);
        bool replace(std::string& str, const std::string& from, const std::string& to);
        std::string to_string (const int & t);
        std::string generatePath(const std::string timezone, const std::string & subDirectory = "");
        std::string getTimestamp();
        std::string getMicroseconds();
        const std::string currentDateTime(std::string timezone = "");
        int compareTime(const std::string & first, const std::string & second);
        const std::string getRootDirectory(const char * relativeDirectory);
        void tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters = ",");
        std::string normalizePath(const std::string & cwd, const std::string & command, const std::string & binaryPath);
    }
}

#endif