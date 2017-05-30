#include "capture/Image.h"
#include <iostream>

namespace kerberos
{
    bool Image::save(const char * fileName)
    {
        std::string name = fileName;
        return save(name);
    }
    
	bool Image::save(const std::string pathToFile)
	{
        try
        {
            return imwrite(pathToFile, m_image);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
	}

    void Image::createMask(int width, int height, PointVector & points)
    {
        cv::Mat mat;
        mat = cv::Mat::zeros(height, width, CV_8UC3);
        cv::Vec3b black(255,255,255);
        
        for(int i = 0; i < points.size(); i++)
        {
            mat.at<cv::Vec3b>(points[i]) = black;
        }

        setImage(mat);
    }
    
    void Image::drawRectangle(JSONValue & region, int color[3])
    {
        try
        {
            cv::Scalar _color(color[0],color[1],color[2]);
            cv::Rect rectangle;    
            rectangle.x = region[0].GetInt();
            rectangle.y = region[1].GetInt();
            rectangle.width = region[2].GetInt() - region[0].GetInt();
            rectangle.height = region[3].GetInt() - region[1].GetInt();
            cv::rectangle(m_image, rectangle, _color, 1);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
	void Image::convert2Gray()
	{
        try
        {
            cvtColor(m_image, m_image, CV_RGB2GRAY);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
	}
    
    void Image::rotate(int angle)
	{
        try
        {
            switch (angle % 360)
            {
                case 0:
                    break;
                case 90:         
                    cv::flip(m_image.t(), m_image, 1);
                    break;
                case 180:
                    cv::flip(m_image, m_image, -1);
                    break;
                case 270:
                    cv::flip(m_image.t(), m_image, 0);
                    break;
                default:
                    break;
            }
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
	}
    
    Image Image::scaleToSmall()
	{
        try
        {
    		Image scaled;
    		return scaled;
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
	}
	
	Image Image::crop(int x1, int y1, int x2, int y2)
	{
        try
        {
            cv::Point tl(x1,y1);
            cv::Point br(x2,y2);
            cv::Rect rectangle(tl,br);
            
    		Image cropped;
            cropped.setImage(m_image(rectangle));
    		return cropped;
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
	}
    
    void Image::difference(const Image & image, Image & result)
    {
        try
        {
            cv::absdiff(m_image, image.m_image, result.m_image);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void Image::bitwiseAnd(const Image & image, Image & result)
    {
        try
        {
            cv::bitwise_and(m_image, image.m_image, result.m_image);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void Image::threshold(const int threshold)
    {
        try
        {
            cv::threshold(m_image, m_image, threshold, 255, CV_THRESH_BINARY);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void Image::erode(const Image & kernel)
    {
        try
        {
            cv::erode(m_image, m_image, kernel.m_image);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void Image::dilate(const Image & kernel)
    {
        try
        {
            cv::dilate(m_image, m_image, kernel.m_image);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    int Image::brightness()
    {
        try
        {
            cv::Scalar mean = cv::mean(m_image);
            return mean[0];
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
}