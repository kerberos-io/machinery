#include "capture/USBCamera.h"

namespace kerberos
{
    void USBCamera::setup(kerberos::StringMap &settings)
    {
        int width = std::atoi(settings.at("captures.USBCamera.frameWidth").c_str());
        int height = std::atoi(settings.at("captures.USBCamera.frameHeight").c_str());
        int deviceNumber = std::atoi(settings.at("captures.USBCamera.deviceNumber").c_str());
        int angle = std::atoi(settings.at("captures.USBCamera.angle").c_str());
        int delay = std::atoi(settings.at("captures.USBCamera.delay").c_str());
        
        // Save width and height in settings
        Capture::setup(settings, width, height);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);
        setDeviceNumber(deviceNumber);
        
        // Initialize USB Camera
        open();
        
        // Initialize executor (update the usb camera at specific times).
        tryToUpdateCapture.setAction(this, &USBCamera::update);
        tryToUpdateCapture.setInterval("thrice in 10 functions calls");
    }
    
    USBCamera::USBCamera(int width, int height)
    {
        try
        {
            m_camera = new cv::VideoCapture(getDeviceNumber());
            setImageSize(width, height);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    };
    
    Image * USBCamera::takeImage()
    {
        // Update camera, call executor's functor.
        tryToUpdateCapture();
        
        // Take image
        try
        {
            // Delay camera for some time..
            usleep(m_delay*1000);
            
            // Get image from camera
            Image * image = new Image();
            m_camera->grab();
            m_camera->grab();
            m_camera->grab();
            m_camera->grab();
            m_camera->grab(); // workaround for buffering
            m_camera->retrieve(image->getImage());
            
            // Check if need to rotate the image
            image->rotate(m_angle);
            
            return image;
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }
    
    void USBCamera::setImageSize(int width, int height)
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
    
    void USBCamera::open()
    {
        m_camera->open(getDeviceNumber());
    }
    
    void USBCamera::close()
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
    
    void USBCamera::update(){}
}