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
        
        // Initialize executor (update the usb camera at specific times).
        tryToUpdateCapture.setAction(this, &IPCamera::update);
        tryToUpdateCapture.setInterval("thrice in 10 functions calls");
        
        // Save width and height in settings
        Capture::setup(settings, width, height, angle);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);

        // Initialize URL to IP Camera
        setUrl(url);
        reopen();
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
    
    void IPCamera::grab()
    {
        try
        {
            pthread_mutex_lock(&m_lock);

            if(!m_camera->grab())
            {
                reopen();
                usleep(1000*500);
            }

            pthread_mutex_unlock(&m_lock);
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&m_lock);
            pthread_mutex_destroy(&m_lock);
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    Image IPCamera::retrieve()
    {
        try
        {
            Image image;
            pthread_mutex_lock(&m_lock);

            if(m_streamType == "rtsp")
            {
                m_camera->retrieve(image.getImage());
            }
            else
            {
                m_camera->read(image.getImage());
            }

            pthread_mutex_unlock(&m_lock);
            return image;
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&m_lock);
            pthread_mutex_destroy(&m_lock);
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    
    Image * IPCamera::takeImage()
    {
        // ----------------------------------------
        // Update camera, call executor's functor.
        
        tryToUpdateCapture();
        
        // -----------
        // Take image

        try
        {
            // -----------------------------------
            // Get image from RTSP or MJPEG stream

            Image * image = new Image();


            while(image->getColumns() == 0 || image->getRows() == 0)
            {
                // ----------------------------
                // Delay camera for some time..

                usleep(m_delay*1000);
            
                pthread_mutex_lock(&m_lock);
                if(m_streamType == "rtsp")
                {
                    m_camera->retrieve(image->getImage());
                }
                else
                {
                    m_camera->read(image->getImage());
                }

                // ---------------------------------
                // Check if need to rotate the image

                image->rotate(m_angle);
                
                pthread_mutex_unlock(&m_lock);
            }
            
            return image;
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&m_lock);
            pthread_mutex_destroy(&m_lock);
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void IPCamera::setImageSize(int width, int height)
    {
        Capture::setImageSize(width, height);
        try
        {
            m_camera->set(CV_CAP_PROP_FRAME_WIDTH, m_frameWidth);
            m_camera->set(CV_CAP_PROP_FRAME_HEIGHT, m_frameHeight);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void IPCamera::setUrl(std::string url)
    {
        m_url=url;
        m_streamType = url.substr(0, 4);
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
        try
        {
            m_camera->open(url);
            setImageSize(m_frameWidth, m_frameHeight);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    void IPCamera::reopen()
    {
        m_camera->release();
        open(m_url.c_str());
        
        if(!m_camera->isOpened())
        {
            m_url += "?"; // retry with ?

            open(m_url.c_str());
        
            if(!m_camera->isOpened())
            {
                throw OpenCVException("can't open url of ip camera");
            }
        }
    }
    
    void IPCamera::close()
    {
        try
        {
            pthread_mutex_unlock(&m_lock);
            pthread_mutex_destroy(&m_lock);
            m_camera->release();
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void IPCamera::update(){}
    
    bool IPCamera::isOpened()
    {
        return m_camera->isOpened();
    }
}