#include "capture/IPCamera.h"

namespace kerberos
{
    void IPCamera::setup(kerberos::StringMap &settings)
    {
        std::string url = settings.at("captures.IPCamera.url");
        int width = std::atoi(settings.at("captures.IPCamera.frameWidth").c_str());
        int height = std::atoi(settings.at("captures.IPCamera.frameHeight").c_str());
        int angle = std::atoi(settings.at("captures.IPCamera.angle").c_str());
        int delay = std::atoi(settings.at("captures.IPCamera.delay").c_str());
        
        // Save width and height in settings
        Capture::setup(settings, width, height);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);
        
        // Initialize URL to IP Camera
        setUrl(url);
        
        // Initialize executor (update the usb camera at specific times).
        tryToUpdateCapture.setAction(this, &IPCamera::update);
        tryToUpdateCapture.setInterval("thrice in 10 functions calls");
    }
    
    IPCamera::IPCamera(int width, int height)
    {
        try
        {
            m_camera = new cv::VideoCapture();
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    };
    
    Image * IPCamera::takeImage()
    {
        // Update camera, call executor's functor.
        tryToUpdateCapture();
        
        // Take image
        try
        {
            // Delay camera for some time..
            usleep(m_delay*1000);
            
            open(m_url.c_str());
            cv::Mat img;
            m_camera->read(img);
            close();
            
            Image * image = new Image();
            image->setImage(img);
            
            // Check if need to rotate the image
            image->rotate(m_angle);
            
            return image;
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void IPCamera::setImageSize(int width, int height)
    {
        Capture::setImageSize(width, height);
    }
    
    void IPCamera::setRotation(int angle)
    {
        Capture::setRotation(angle);
    }
    
    void IPCamera::setDelay(int msec)
    {
        Capture::setDelay(msec);
    }
    
    void IPCamera::open(){}
    void IPCamera::open(const char * url)
    {
        m_camera->open(url);
    }
    
    void IPCamera::close()
    {
        try
        {
            m_camera->release();
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void IPCamera::update(){}
}