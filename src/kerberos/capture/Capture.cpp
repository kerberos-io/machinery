#include "capture/Capture.h"
    
namespace kerberos
{
    void Capture::setup(kerberos::StringMap & settings, int width, int height, int angle)
    {
        // --------------------------
        // Make width & height global.
        
        settings["capture.width"] = helper::to_string(width);
        settings["capture.height"] = helper::to_string(height);
        settings["capture.angle"] = helper::to_string(angle);
        
        // ----------------
        // Initialize mutex
        
        pthread_mutex_init(&m_lock, NULL);
    }
        
    void Capture::setImageSize(int width, int height)
    {
        m_frameWidth = width;
        m_frameHeight = height;
    }
    
    void Capture::setRotation(int angle)
    {
        m_angle = angle;
    }
    
    void Capture::setDelay(int msec)
    {
        m_delay = msec;   
    }
    
    ImageVector & Capture::takeImages(int numberOfImages)
    {
	    m_images.resize(numberOfImages);
	    for(int i = 0; i < numberOfImages; i++)
	    {
	    	m_images[i] = takeImage();
		}
        return m_images;
    }

    ImageVector & Capture::shiftImage()
    {
        return shiftImages(1);
    }

    ImageVector & Capture::shiftImages(int numberOfImages)
    {
        // -------------
        // Delete images

        for(int i = 0; i < numberOfImages; i++)
        {
            delete m_images[i];
        }

        // -------------
        // Shift images

        for(int i = numberOfImages; i <= m_images.size()-numberOfImages; i++)
        {
            m_images[i-numberOfImages] = m_images[i];
        }

        // -------------
        // Take images
        
        for(int i = m_images.size()-numberOfImages; i < m_images.size(); i++)
        {
            m_images[i] = takeImage();
        }
        return m_images;
    }
    
    // -------------------------------------------
    // Function ran in a thread, which continously
    // grabs frames.
    
    void * grabContinuously(void * self)
    {
        Capture * capture = (Capture *) self;

        for(;;)
        {
            if(capture->isOpened())
            {
                try
                {
                    capture->grab();
                }
                catch(cv::Exception & ex)
                {
                    LERROR << ex.what();
                }
            }
            usleep(333*100);
        }
    }
    
    void Capture::startGrabThread()
    {
        // ------------------------------------------------
        // Start a new thread that grabs images continously.
        // This is needed to clear the buffer of the capture device.
        
        pthread_create(&m_captureThread, NULL, grabContinuously, this);   
    }
    
    void Capture::stopGrabThread()
    {
        // ----------------------------------
        // Cancel the existing capture thread,
        // before deleting the device.
        
        pthread_cancel(m_captureThread);
    }
}