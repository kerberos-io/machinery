//
//  Class: IoVideo
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

#ifndef __IoVideo_H_INCLUDED__   // if IoVideo.h hasn't been included yet...
#define __IoVideo_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"
#include <stdlib.h>
#include <time.h>
#include <pthread.h> //for threading , link with lpthread

namespace kerberos
{
    char VideoName[] = "Video";
    class IoVideo : public IoCreator<VideoName, IoVideo>
    {
        private:
            std::string m_instanceName;
            std::string m_fileFormat;
            bool m_drawTimestamp;
            cv::Scalar m_timestampColor;
            std::string m_timezone;
            FileManager m_fileManager;

        public:
            IoVideo(){};
            ~IoVideo()
            {
                stopRecordThread();
            }; 

            void setup(const StringMap & settings);
            void fire();
            void disableCapture();
            std::string buildPath(std::string pathToVideo);
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
            int getFPS(){return m_fps;};
            void setFPS(int fps){m_fps = fps;};
            bool save(Image & image);
            bool save(Image & image, JSON & data);
            
            cv::VideoWriter * m_writer;
            Image m_mostRecentImage;
            bool m_recording;
            pthread_mutex_t m_lock;
            pthread_mutex_t m_time_lock;
            pthread_t m_recordThread;
            pthread_t m_retrieveThread;
            double m_timeStartedRecording;
            void startRecordThread();
            void stopRecordThread();
            void startRetrieveThread();
            void stopRetrieveThread();

            int m_codec;
            int m_fps;
            int m_recordingTimeAfter;
            int m_width;
            int m_height;
            std::string m_extension;
            std::string m_directory; 
    };
}
#endif