#include "capture/RaspiCamera.h"

namespace kerberos
{
    void RaspiCamera::setup(kerberos::StringMap &settings)
    {
        int width = std::atoi(settings.at("captures.RaspiCamera.frameWidth").c_str());
        int height = std::atoi(settings.at("captures.RaspiCamera.frameHeight").c_str());
        int angle = std::atoi(settings.at("captures.RaspiCamera.angle").c_str());
        int delay = std::atoi(settings.at("captures.RaspiCamera.delay").c_str());

        // Save width and height in settings.
        Capture::setup(settings, width, height);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);
        
        // Open camera
        open();
        
        // Initialize executor
        tryToUpdateCapture.setAction(this, &RaspiCamera::update);
        tryToUpdateCapture.setInterval("once at 1000 calls");
    }
    
    void RaspiCamera::grab()
    {
        try
        {
            pthread_mutex_lock(&m_lock);
            m_camera->grab();
            pthread_mutex_unlock(&m_lock);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    Image RaspiCamera::retrieve()
    {
        try
        {
            Image image;
            pthread_mutex_lock(&m_lock);
            m_camera->retrieve(image.getImage());
            pthread_mutex_unlock(&m_lock);
            return image;
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    Image * RaspiCamera::takeImage()
    {
        // update the camera settings, with latest images
        //  - it's possible that we have to change the brightness, saturation, etc.
        tryToUpdateCapture();
        
        // Delay camera for some time..
        usleep(m_delay*1000);
        
        // take an image 
        Image * image = new Image();
        
        pthread_mutex_lock(&m_lock);
        m_camera->grab();
        m_camera->retrieve(image->getImage());
        pthread_mutex_unlock(&m_lock);
        
        // Check if need to rotate the image
        image->rotate(m_angle);
                
        return image;
    }
    
    void RaspiCamera::setImageSize(int width, int height)
    {
        Capture::setImageSize(width, height);
        m_camera->set(CV_CAP_PROP_FORMAT, CV_8UC3);
        m_camera->set(CV_CAP_PROP_FRAME_WIDTH, m_frameWidth);
        m_camera->set(CV_CAP_PROP_FRAME_HEIGHT, m_frameHeight);
    }
    
    void RaspiCamera::setRotation(int angle)
    {
        Capture::setRotation(angle);
    }
    
    void RaspiCamera::setDelay(int msec)
    {
        Capture::setDelay(msec);
    }
    
    void RaspiCamera::open()
    {
        m_camera->open();
    }
    
    void RaspiCamera::close()
    {
        m_camera->release();
    }
    
    void RaspiCamera::update(){}
}
