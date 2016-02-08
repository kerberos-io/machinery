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
        reopen();

        // Initialize executor (update the usb camera at specific times).
        tryToUpdateCapture.setAction(this, &IPCamera::update);
        tryToUpdateCapture.setInterval("thrice in 10 functions calls");
        
        // Start connection thread
        startConnectionThread();
        
        // Initialize mutex
        pthread_mutex_init(&m_connectionLock, NULL);
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
            m_camera->grab();
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
            m_camera->retrieve(image.getImage());
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
        // Update camera, call executor's functor.
        tryToUpdateCapture();
        
        // Take image
        try
        {
            // Delay camera for some time..
            usleep(m_delay*1000);
            
            // Get image from RTSP or MJPEG stream
            Image * image = new Image();
            
            pthread_mutex_lock(&m_lock);
            pthread_mutex_lock(&m_connectionLock);
            if(m_streamType == "rtsp")
            {
                m_camera->grab();
                m_camera->retrieve(image->getImage());
            }
            else
            {
                open(m_url.c_str());
                m_camera->read(image->getImage());
                close();
            }
            pthread_mutex_unlock(&m_connectionLock);
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
            if(!isOpened())
            {
                m_camera->release();
                m_camera->open(url);
            }
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    void IPCamera::reopen()
    {
        while(!isOpened())
        {
            open(m_url.c_str());
            usleep(1000*2500);
        }
    }
    
    void IPCamera::close()
    {
        try
        {
            closeConnectionThread();
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
    
    // -------------------------------------------
    // Function ran in a thread, which continously
    // grabs frames.
    
    void * checkConnection(void * self)
    {
        IPCamera * capture = (IPCamera *) self;
        
        int count = capture->m_connectionCount;
        for(;;)
        {
            usleep(1000*1000);
            count += 1;
            count %=  1024;
            
            if(count == capture->m_connectionCount)
            {
                pthread_mutex_lock(&capture->m_connectionLock);
                capture->stopGrabThread();
                capture->reopen();
                pthread_mutex_unlock(&capture->m_connectionLock);
                pthread_mutex_unlock(&capture->m_lock);
                capture->startGrabThread();
            }
        }
    }
    void IPCamera::startConnectionThread()
    {   
        m_connectionCount = 0;
        pthread_create(&m_connectionThread, NULL, checkConnection, this); 
    }
    
    void IPCamera::closeConnectionThread()
    {   
        pthread_detach(m_connectionThread);
        pthread_cancel(m_connectionThread);  
    }
}