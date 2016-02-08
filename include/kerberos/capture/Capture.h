//
//  Class: Capture
//  Description: Devices to capture images (video, webcam, raspberry cam, etc).
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//	Website:	 www.cedric.ws
//
//  The copyright to the computer program(s) herein
//  is the property of Cédric Verstraeten, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#include "Factory.h"
#include "capture/Image.h"

#ifndef __Capture_H_INCLUDED__   // if Capture.h hasn't been included yet...
#define __Capture_H_INCLUDED__   // #define this so the compiler knows it has been included

namespace kerberos
{
    class Capture
    {
        protected:
            ImageVector m_images;
            const char * name;
        
        public:
            pthread_mutex_t m_lock;
            pthread_t m_captureThread;
            
            int m_frameWidth, m_frameHeight;
            int m_angle; // 90, 180, 270
            int m_delay; // msec
        
            Capture(){};
            virtual ~Capture(){};
            virtual void setup(kerberos::StringMap & settings) = 0;
            void setup(kerberos::StringMap & settings, int width, int height);
            virtual void setImageSize(int width, int height);
            virtual void setRotation(int angle);
            virtual void setDelay(int msec);
              
            virtual void grab() = 0;
            virtual Image retrieve() = 0;
            virtual Image * takeImage() = 0;

            ImageVector & takeImages(int numberOfImages);
            ImageVector & shiftImage();
            ImageVector & shiftImages(int numberOfImages);
        
            virtual void open() = 0;
            virtual void close() = 0;
            virtual void update() = 0;
            virtual bool isOpened() = 0;
        
            void startGrabThread();
            void stopGrabThread();
    };

    template<const char * Alias, typename Class>
    class CaptureCreator: public Capture
    {
        protected:
            CaptureCreator(){name = ID;}
            
        public:
            static Capture* create(){return new Class();};
            static const char * ID;
    };
}
#endif
