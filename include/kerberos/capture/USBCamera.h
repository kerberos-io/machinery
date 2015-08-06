//
//  Class: USBCamera
//  Description: Class that handles USB cameras.
//  Created:     11/09/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.cedric.ws
//
//  The copyright to the computer program(s) herein
//  is the property of Cédric Verstraeten, Belgium.
//  The program(s) may be used and/or copied .
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
            CvCapture * m_camera;
            Executor<USBCamera> tryToUpdateCapture;
        
        public:
            USBCamera()
            {
                try
                {
                    m_camera = cvCaptureFromCAM(CV_CAP_ANY);
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
            void setRotation(int angle);
            void setDelay(int msec);
        
            Image * takeImage();
        
            void open();
            void close();
        
            void update();
    };
}

#endif