//
//  Class: IoDisk
//  Description: Save information to disk (an image)
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#ifndef __IoDisk_H_INCLUDED__   // if IoDisk.h hasn't been included yet...
#define __IoDisk_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"

namespace kerberos
{
    char DiskName[] = "Disk";
    class IoDisk : public IoCreator<DiskName, IoDisk>
    {
        private:
            std::string m_instanceName;
            std::string m_fileFormat;
            FileManager m_fileManager;
            
        public:
            IoDisk(){};
            void setup(const StringMap & settings);
            void setInstanceName(std::string instanceName){m_instanceName=instanceName;};
            std::string getInstanceName(){return m_instanceName;};
            std::string getFileFormat(){return m_fileFormat;};
            void setFileFormat(std::string fileFormat){m_fileFormat = fileFormat;};
            std::string buildPath(std::string pathToImage);
            bool save(Image & image);
            bool save(Image & image, JSON & data);
    };
}
#endif