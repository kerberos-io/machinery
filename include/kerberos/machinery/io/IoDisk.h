//
//  Class: IoDisk
//  Description: Save information to disk (an image)
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

#ifndef __IoDisk_H_INCLUDED__   // if IoDisk.h hasn't been included yet...
#define __IoDisk_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"

namespace kerberos
{
    char DiskName[] = "Disk";
    class IoDisk : public IoCreator<DiskName, IoDisk>
    {
        typedef std::vector<Point2f> PointVector;

        private:
            std::string m_instanceName;
            std::string m_fileFormat;
            bool m_drawTimestamp;
            cv::Scalar m_timestampColor;
            std::string m_timezone;
            FileManager m_fileManager;
            std::string m_publicKey;
            std::string m_privateKey;
            bool m_createSymbol;
            Image m_mask;
            bool m_privacy;

        public:
            IoDisk(){};
            void setup(const StringMap & settings);
            void fire(JSON & data){};
            void disableCapture(){};
            cv::Scalar getColor(const std::string name);
            bool getDrawTimestamp(){return m_drawTimestamp;};
            void setDrawTimestamp(bool drawTimestamp){m_drawTimestamp=drawTimestamp;};
            std::string getTimezone(){return m_timezone;};
            void setTimezone(std::string timezone){m_timezone=timezone;};
            cv::Scalar getTimestampColor(){return m_timestampColor;};
            void setTimestampColor(cv::Scalar timestampColor){m_timestampColor=timestampColor;};
            std::string getInstanceName(){return m_instanceName;};
            void setInstanceName(std::string instanceName){m_instanceName=instanceName;};
            std::string getFileFormat(){return m_fileFormat;};
            void setFileFormat(std::string fileFormat){m_fileFormat = fileFormat;};
            std::string buildPath(std::string pathToImage);
            void drawDateOnImage(Image & image, std::string timestamp);
            bool save(Image & image);
            bool save(Image & image, JSON & data);
    };
}
#endif