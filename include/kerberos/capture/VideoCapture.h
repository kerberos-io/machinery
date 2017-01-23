//
//  Class: VideoCapture
//  Description: Class that plays a video file.
//  Created:     17/05/2016
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

#ifndef __VideoCapture_H_INCLUDED__   // if VideoCapture.h hasn't been included yet...
#define __VideoCapture_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "capture/Capture.h"
#include "Executor.h"

namespace kerberos
{
    char VideoCaptureName[] = "VideoCapture";
    class VideoCapture : public CaptureCreator<VideoCaptureName, VideoCapture>
    {
        private:
            cv::VideoCapture * m_video;
            std::string m_path;
        
        public:
            VideoCapture()
            {
                try
                {
                    m_video = new cv::VideoCapture();
                }
                catch(cv::Exception & ex)
                {
                    throw OpenCVException(ex.msg.c_str());
                }
            }
        
            VideoCapture(int width, int height);
            virtual ~VideoCapture(){};
            void setup(StringMap & settings);
            void setImageSize(int width, int height);
            void setRotation(int angle){Capture::setRotation(angle);}
            void setDelay(int msec){Capture::setDelay(msec);}
            void setPath(std::string path){m_path=path;}
            std::string getPath(){return m_path;}
            
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