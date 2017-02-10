//
//  Class: USBCamera
//  Description: Class that handles USB cameras.
//  Created:     11/09/2014
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

#ifndef __USBCamera_H_INCLUDED__   // if USBCamera.h hasn't been included yet...
#define __USBCamera_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "capture/Capture.h"
#include "Executor.h"

namespace kerberos
{
    char USBCameraName[] = "USBCamera";
    class USBCamera : public CaptureCreator<USBCameraName, USBCamera>
    {
        private:
            cv::VideoCapture * m_camera;
            Executor<USBCamera> tryToUpdateCapture;
            int m_deviceNumber;
        
        public:
            USBCamera()
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
        
            USBCamera(int width, int height);
            virtual ~USBCamera(){};
            void setup(StringMap & settings);
            void setImageSize(int width, int height);
            void setRotation(int angle){Capture::setRotation(angle);}
            void setDelay(int msec){Capture::setDelay(msec);}
            void setDeviceNumber(int number){m_deviceNumber=number;}
            int getDeviceNumber(){return m_deviceNumber;}
            
            void grab();
            Image retrieve();
            Image * takeImage();
        
            void open();
            void close();
            void update();
            bool isOpened();
    };
}

#endif