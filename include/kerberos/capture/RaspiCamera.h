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
#include "Executor.h"
#include "Camera.h"
#include "VideoEncode.h"
#include "VideoDecode.h"
#include "VideoRender.h"
#include <map>

namespace kerberos
{
    char RaspiCameraName[] = "RaspiCamera";
    class RaspiCamera : public CaptureCreator<RaspiCameraName, RaspiCamera>
    {
        private:
            Executor<RaspiCamera> tryToUpdateCapture;
            int m_brightness;
            int m_contrast;
            int m_saturation;
            int m_sharpness;

        public:
            RaspiCamera(){}
            ~RaspiCamera();
            void setup(StringMap & settings);
            void setImageSize(int width, int height);
            void setRotation(int angle);
            void setDelay(int msec);

            uint8_t * mjpeg_data_buffer;
            int32_t mjpeg_data_length;

            uint8_t * data_buffer;
            int32_t data_length;
            uint8_t * image_data;

            void grab();
            Image retrieve();
            int32_t retrieveRAW(uint8_t* data);
            Image * takeImage();
            void startRecord(std::string path);
            void stopRecord();
            void stopThreads();

            void open();
            void close();
            void update();
            bool isOpened();
    };
}

#endif
