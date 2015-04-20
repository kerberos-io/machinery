#include "capture/RaspiCamera.h"

namespace kerberos
{
    void RaspiCamera::setup(kerberos::StringMap &settings)
    {
        int width = std::atoi(settings.at("captures.RaspiCamera.frameWidth").c_str());
        int height = std::atoi(settings.at("captures.RaspiCamera.frameHeight").c_str());
        m_toggle.night = std::atoi(settings.at("captures.RaspiCamera.night").c_str());
        m_toggle.day = std::atoi(settings.at("captures.RaspiCamera.day").c_str());

        // Save width and height in settings.
        Capture::setup(settings, width, height);
        setImageSize(width, height);
        
        // Open camera, with default brightness settings.
        open();
        
        // Initialize executor
        tryToUpdateCapture.setAction(this, &RaspiCamera::update);
        tryToUpdateCapture.setInterval("once at 1000 calls");
        
        // Calculate the mode, this will restart the camera with the correct settings.
        Image * snapshot = grab();
        m_toggle.isNight = false; // the capture device is loaded, start with the daily settings.
        m_toggle.isNight = isNight(snapshot); // do a day or night check, works with a gray zone; see documentation.
        if(m_toggle.isNight)
        {
            toggleDayOrNight();
        }
        delete snapshot;
    }
    
    Image * RaspiCamera::takeImage()
    {
        // update the camera settings, with latest images
        //  - it's possible that we have to change the brightness, saturation, etc.
        tryToUpdateCapture();
        
        // take an image 
        Image * image = grab();
        
        return image;
    }
    
    Image * RaspiCamera::grab()
    {
        Image * image = new Image();
        m_camera->grab();
        m_camera->retrieve(image->getImage());
        return image;
    }
    
    void RaspiCamera::setImageSize(int width, int height)
    {
        Capture::setImageSize(width, height);
        m_camera->set(CV_CAP_PROP_FORMAT, CV_8UC3);
        m_camera->set(CV_CAP_PROP_FRAME_WIDTH, m_frameWidth);
        m_camera->set(CV_CAP_PROP_FRAME_HEIGHT, m_frameHeight);
    }
    
    void RaspiCamera::open()
    {
        m_camera->open();
    }
    
    void RaspiCamera::close()
    {
        m_camera->release();
    }
    
    void RaspiCamera::update()
    {
        // if no image in the queue
        if(m_images[m_images.size()-1] != 0)
        {
            bool night = isNight(m_images[m_images.size()-1]);
        
            // Toggle if switching from day -> night or night -> day
            if(m_toggle.isNight != night)
            {
                m_toggle.isNight = night;
                toggleDayOrNight();
            }
        }
    }
    
    bool RaspiCamera::isNight(Image * image)
    {
        int brightness = image->brightness();
        
        if(brightness < m_toggle.night)
        {
            return true;
        }
        else if(brightness > m_toggle.day)
        {
            return false;
        }
        return m_toggle.isNight;
    }
    
    void RaspiCamera::toggleDayOrNight()
    {
        close();

        if(m_toggle.isNight)
        {
            setNightMode();
        }
        else
        {
            setDayMode();
        }
        
        open();
    }
    
    void RaspiCamera::setNightMode()
    {
        m_camera->set(CV_CAP_PROP_CONTRAST, 58);
        m_camera->set(CV_CAP_PROP_BRIGHTNESS, 57);
    }
    
    void RaspiCamera::setDayMode()
    {
        m_camera->set(CV_CAP_PROP_CONTRAST, 50);
        m_camera->set(CV_CAP_PROP_BRIGHTNESS, 50);
    }
}
