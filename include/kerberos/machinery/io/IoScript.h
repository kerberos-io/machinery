//
//  Class: IoScript
//  Description: A Script class that will execute
//  a bash script.
//  Created:     23/01/2017
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

#ifndef __IoScript_H_INCLUDED__   // if IoScript.h hasn't been included yet...
#define __IoScript_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"
#include "document.h"
#include "writer.h"

namespace kerberos
{
    char ScriptName[] = "Script";
    class IoScript : public IoCreator<ScriptName, IoScript>
    {
        private:
            std::string m_path;
            std::string m_instanceName;

        public:
            IoScript(){};
            void setup(const StringMap & settings);
            void fire(JSON & data){};
            void disableCapture(){};
            
            void setPath(std::string path){m_path=path;};
            const char * getPath(){return m_path.c_str();};
            void setInstanceName(std::string instanceName){m_instanceName=instanceName;};
            std::string getInstanceName(){return m_instanceName;};
            
            bool save(Image & image){ return true; };
            bool save(Image & image, JSON & data);
    };
}
#endif
