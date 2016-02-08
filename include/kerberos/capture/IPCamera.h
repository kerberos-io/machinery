//
//  Class: IPCamera
//  Description: Class that handles an IP camera.
//  Created:     23/07/2015
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//	Website:	 www.cedric.ws
//
//  The copyright to the computer program(s) herein
//  is the property of Cédric Verstraeten, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#ifndef __IPCamera_H_INCLUDED__   // if IPCamera.h hasn't been included yet...
#define __IPCamera_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "capture/Capture.h"
#include "Executor.h"

namespace kerberos
{
    char IPCameraName[] = "IPCamera";
    class IPCamera : public CaptureCreator<IPCameraName, IPCamera>
    {
        private:
            cv::VideoCapture * m_camera;
            Executor<IPCamera> tryToUpdateCapture;
            std::string m_url;
            std::string m_streamType;
        
        public:   
            int m_connectionCount;
            pthread_mutex_t m_connectionLock;
            pthread_t m_connectionThread;
        
            IPCamera()
            {
                try
                {
                    m_camera = new cv::VideoCapture();
                }
                catch(cv::Exception & ex)
                {
                    throw OpenCVException(ex.msg.c_str());
                }
            }
        
            IPCamera(int width, int height);
            virtual ~IPCamera(){};
            void setup(StringMap & settings);
            void setImageSize(int width, int height);
            void setUrl(std::string url);
            void setRotation(int angle);
            void setDelay(int msec);
        
            void grab();
            Image retrieve();
            Image * takeImage();
        
            void open();
            void open(const char * url);
            void reopen();
            void close();
            void update();
            bool isOpened();
        
            void startConnectionThread();
            void closeConnectionThread();
    };
}

#endif