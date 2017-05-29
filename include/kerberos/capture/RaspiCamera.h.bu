//
//  Class: RaspiCamera
//  Description: Class that handles the raspberry pi camera.
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

#ifndef __RaspiCamera_H_INCLUDED__   // if RaspiCamera.h hasn't been included yet...
#define __RaspiCamera_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "capture/Capture.h"
#include "raspicam/raspicam_cv.h"
#include "Executor.h"

namespace kerberos
{
    char RaspiCameraName[] = "RaspiCamera";
    class RaspiCamera : public CaptureCreator<RaspiCameraName, RaspiCamera>
    {
        private:
            raspicam::RaspiCam_Cv * m_camera;
            Executor<RaspiCamera> tryToUpdateCapture;
        
        public:
            RaspiCamera()
            {
                m_camera = new raspicam::RaspiCam_Cv();
            }
            void setup(StringMap & settings);
            void setImageSize(int width, int height);
            void setRotation(int angle);
            void setDelay(int msec);
            
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