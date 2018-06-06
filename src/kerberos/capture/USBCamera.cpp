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
        std::string fourcc = settings.at("captures.USBCamera.fourcc");

        // Initialize executor (update the usb camera at specific times).
        tryToUpdateCapture.setAction(this, &USBCamera::update);
        tryToUpdateCapture.setInterval("thrice in 10 functions calls");

        // Save width and height in settings
        Capture::setup(settings, width, height, angle);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);
        setDeviceNumber(deviceNumber);
        setFourcc(fourcc);

        // Initialize USB Camera
        open();
    }

    USBCamera::USBCamera(int width, int height)
    {
        try
        {
            m_camera = new cv::VideoCapture();
            setImageSize(width, height);
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    };

    void USBCamera::grab()
    {
        try
        {
            pthread_mutex_lock(&m_lock);
            incrementHealth();
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

    Image USBCamera::retrieve()
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

            pthread_mutex_lock(&m_lock);
            m_camera->grab();
            m_camera->retrieve(image->getImage());
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


    void USBCamera::setImageSize(int width, int height)
    {
        Capture::setImageSize(width, height);
        try
        {
            m_camera->set(CV_CAP_PROP_FRAME_WIDTH, m_frameWidth);
            m_camera->set(CV_CAP_PROP_FRAME_HEIGHT, m_frameHeight);
            m_camera->set(CV_CAP_PROP_FOURCC, CV_FOURCC(m_fourcc[0], m_fourcc[1], m_fourcc[2], m_fourcc[3]));
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }
    }

    void USBCamera::open()
    {
        LINFO << "Capture: Trying to open USB camera.";
        LINFO << "Capture: (Warning) You can change the capture device with the configuration files.";

        try
        {
            if(!isOpened())
            {
                m_camera->release();
                m_camera->open(getDeviceNumber());
                setImageSize(m_frameWidth, m_frameHeight);

                if(!isOpened())
                {
                    throw OpenCVException("can't open usb camera");
                }
            }
        }
        catch(cv::Exception & ex)
        {
            throw OpenCVException(ex.msg.c_str());
        }

        LINFO << "Capture: Succesfully opened USB camera.";
    }

    void USBCamera::close()
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

    void USBCamera::update(){}

    bool USBCamera::isOpened()
    {
        return m_camera->isOpened();
    }
}
