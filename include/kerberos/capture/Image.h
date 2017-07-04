//
//  Class: Image.h
//  Description: An image for simple image manipulations, an OpenCV wrapper.
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        cedric@verstraeten.io
//  Website:     www.verstraeten.io
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium. 
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#ifndef __Image_H_INCLUDED__   // if Image.h hasn't been included yet...
#define __Image_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Types.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Exception.hpp"

namespace kerberos
{
    typedef cv::Point2f Point2f;
    
    class Image
    {
        typedef std::vector<Point2f> PointVector;

        private:
            cv::Mat m_image;

        public:
            Image(){};
            virtual ~Image(){};
            Image & operator=(const Image & img)
            {
                m_image = img.m_image;
                return *this;
            };

            cv::Mat & getImage(){return m_image;};
            virtual void setImage(const cv::Mat & image){m_image = image;};
            virtual int get(int y, int x){return m_image.at<uchar>(y,x);};
            virtual int getColumns(){return m_image.cols;};
            virtual int getRows(){return m_image.rows;};
            bool save(const std::string fileName);
            bool save(const char * fileName);
		
            void drawRectangle(JSONValue & region, int color[3]);
            void convert2Gray();
            void rotate(int angle);
            void difference(const Image & image, Image & result);
            void bitwiseAnd(const Image & image, Image & result);
            void erode(const Image & kernel);
            void dilate(const Image & kernel);
            void threshold(const int threshold);
            void createMask(int width, int height, PointVector & points);
            void drawMask(Image & mask);

            int brightness();
            static cv::Mat createKernel(int width, int height)
            {
                return getStructuringElement(cv::MORPH_RECT, cv::Size(width,height));
            };
            Image crop(int x1, int y1, int x2, int y2);
            Image scaleToSmall();
    };
}
#endif 
