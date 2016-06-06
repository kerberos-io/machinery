//
//  Class: BackgroundSubstraction
//  Description: An algorithm to detect motion using background substraction.
//  Created:     17/05/2016
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#ifndef __BackgroundSubstraction_H_INCLUDED__   // if BackgroundSubstraction.h hasn't been included yet...
#define __BackgroundSubstraction_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/algorithm/Algorithm.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/video/video.hpp"

namespace kerberos
{
    char BackgroundSubstractionName[] = "BackgroundSubstraction";
    class BackgroundSubstraction : public AlgorithmCreator<BackgroundSubstractionName, BackgroundSubstraction>
    {
        private:
            int m_threshold;
            Image m_erodeKernel;
            Image m_dilateKernel;
            Image m_backgroud;
            cv::Ptr<cv::BackgroundSubtractorMOG2> m_substractor;

        public:
            BackgroundSubstraction(){}
            void setup(const StringMap & settings);
        
            void setErodeKernel(int width, int height);
            void setDilateKernel(int width, int height);
            void setThreshold(int threshold);
            void initialize(ImageVector & images);
            Image evaluate(ImageVector & images, JSON & data);
    };
}
#endif