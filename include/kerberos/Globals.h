//
//  Class: Version
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

#ifndef __Version_H_INCLUDED__   // if Version.h hasn't been included yet...
#define __Version_H_INCLUDED__   // #define this so the compiler knows it has been included

    #define VERSION "2.3.1"
    #define HADES "https://hades.kerberos.io"
    #define CLOUD "https://cloud.kerberos.io"
    #define SYMBOL_DIRECTORY "/etc/opt/kerberosio/symbols/"
    #define CONFIGURATION_PATH "/etc/opt/kerberosio/config/config.xml"
    #define LOG_PATH "/etc/opt/kerberosio/logs/log.stash"

    #define RUNNING_ON_A_RASPBERRYPI false
    #if IS_RASPBERRYPI == 1
        #define RUNNING_ON_A_RASPBERRYPI true
    #endif

#endif
