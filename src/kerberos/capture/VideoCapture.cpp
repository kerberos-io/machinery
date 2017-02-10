#include "capture/VideoCapture.h"

namespace kerberos
{
    void VideoCapture::setup(kerberos::StringMap &settings)
    {
        int width = std::atoi(settings.at("captures.VideoCapture.frameWidth").c_str());
        int height = std::atoi(settings.at("captures.VideoCapture.frameHeight").c_str());
        std::string path = settings.at("captures.VideoCapture.path");
        int angle = std::atoi(settings.at("captures.VideoCapture.angle").c_str());
        int delay = std::atoi(settings.at("captures.VideoCapture.delay").c_str());
        
        // Save width and height in settings
        Capture::setup(settings, width, height, angle);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);
        setPath(path);
        
        // Initialize video
        open();
    }
    
    VideoCapture::VideoCapture(int width, int height)
    {
        try
        {
            m_video = new cv::VideoCapture();
            setImageSize(width, height);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    };
    
    void VideoCapture::grab()
    {
        try
        {
            pthread_mutex_lock(&m_lock);
            // A video doesn't need a grabber.
            // m_video->grab();
            pthread_mutex_unlock(&m_lock);
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&m_lock);
            pthread_mutex_destroy(&m_lock);
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    Image VideoCapture::retrieve()
    {
        try
        {
            Image image;
            pthread_mutex_lock(&m_lock);
            m_video->retrieve(image.getImage());
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
    
    Image * VideoCapture::takeImage()
    {
        // Take image
        try
        {
            // Delay camera for some time..
            usleep(m_delay*1000);
            cv::waitKey(10); // this is needed for video files.
            
            // Get image from camera
            Image * image = new Image();
            
            pthread_mutex_lock(&m_lock);
            m_video->grab();
            m_video->retrieve(image->getImage());
            pthread_mutex_unlock(&m_lock);
            
            // Check if need to rotate the image
            image->rotate(m_angle);
            
            return image;
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&m_lock);
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    
    void VideoCapture::setImageSize(int width, int height)
    {
        Capture::setImageSize(width, height);
        try
        {
            m_video->set(CV_CAP_PROP_FRAME_WIDTH, m_frameWidth);
            m_video->set(CV_CAP_PROP_FRAME_HEIGHT, m_frameHeight);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void VideoCapture::open()
    {
        try
        {
            if(!isOpened())
            {
                m_video->release();
                m_video->open(getPath());
                
                if(!isOpened())
                {
                    throw OpenCVException("can't open raspberry pi camera");
                }

                setImageSize(m_frameWidth, m_frameHeight);
            }
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void VideoCapture::close()
    {
        try
        {
            pthread_mutex_unlock(&m_lock);
            pthread_mutex_destroy(&m_lock);
            m_video->release();
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void VideoCapture::update(){}
    
    bool VideoCapture::isOpened()
    {
        return m_video->isOpened();
    }
}