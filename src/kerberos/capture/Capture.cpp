#include "capture/Capture.h"

namespace kerberos
{
    void Capture::setup(kerberos::StringMap & settings, int width, int height)
    {
        settings["capture.width"] = helper::to_string(width);
        settings["capture.height"] = helper::to_string(height);
    }
        
    void Capture::setImageSize(int width, int height)
    {
        m_frameWidth = width;
        m_frameHeight = height;
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
}