//
//  Class: System
//  Description: Provide information about
//  the current system.
//  Created:     05/07/2017
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

#include "Globals.h"
#include "Helper.h"
#include <string>
#include <algorithm>

#ifndef __System_H_INCLUDED__   // if System.h hasn't been included yet...
#define __System_H_INCLUDED__   // #define this so the compiler knows it has been included

namespace kerberos
{
    class System
    {
        public:

            std::string static getHostname();
            std::string static getDiskPercentage(std::string partition);
            std::string static getTemperature();
            std::string static getWifiSSID();
            std::string static getWifiStrength();
            std::string static getUptime();
            std::string static getBoard();
            std::string static isDocker();
            std::string static isKiOS();
            std::string static getNumberOfFiles(std::string & directory);
            std::string static getCPUid();
    };
}
#endif
