#include "Kerberos.h"

namespace kerberos
{
    void Kerberos::bootstrap(StringMap & parameters)
    {
        // --------------------------------
        // Set parameters from command-line
        
        setParameters(parameters);
        
        // ----------------
        // Initialize mutex
        
        pthread_mutex_init(&m_streamLock, NULL);
        pthread_mutex_init(&m_cloudLock, NULL);
        
        // ---------------------
        // Initialize kerberos
        
        std::string configuration = (helper::getValueByKey(parameters, "config")) ?: CONFIGURATION_PATH;
        configure(configuration);

        // ------------------
        // Open the stream

        startStreamThread();

        // ------------------
        // Open the io thread

        startIOThread();
        
        // ------------------------------------------
        // Guard is a filewatcher, that looks if the 
        // configuration has been changed. On change 
        // guard will re-configure all instances.

        std::string directory = configuration.substr(0, configuration.rfind('/'));
        std::string file = configuration.substr(configuration.rfind('/')+1);
        guard = new FW::Guard();
        guard->listenTo(directory, file);
        
        guard->onChange(&Kerberos::reconfigure);
        guard->start();
        
        // --------------------------
        // This should be forever...

        while(true)
        {
            // -------------------
            // Initialize data

            JSON data;
            data.SetObject();

            // ------------------------------------
            // Guard look if the configuration has
            // been changed...
            
            guard->look();
            
            // --------------------------------------------
            // If machinery is NOT allowed to do detection
            // continue iteration
            
            if(!machinery->allowed(m_images))
            {
                continue;
            }
            
            // --------------------
            // Clean image to save

            Image cleanImage = *m_images[m_images.size()-1];

            // --------------
            // Processing..
            
            if(machinery->detect(m_images, data))
            {
                pthread_mutex_lock(&m_ioLock);

                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                data.Accept(writer);
                Detection detection(buffer.GetString(), cleanImage);
                m_detections.push_back(detection);

                pthread_mutex_unlock(&m_ioLock);
            }

            // -------------
            // Shift images

            m_images = capture->shiftImage();
            usleep(250*1000);
        }
    }
    
    void Kerberos::configure(const std::string & configuration)
    {
        // ---------------------------
    	// Get settings from XML file
        
        StringMap settings = kerberos::helper::getSettingsFromXML(configuration);
        
        // -------------------------------
        // Override config with parameters
        
        StringMap parameters = getParameters();
        StringMap::iterator begin = parameters.begin();
        StringMap::iterator end = parameters.end();
        
        for(begin; begin != end; begin++)
        {
            settings[begin->first] = begin->second;
        }
        
        // -----------------
        // Configure cloud
        
        configureCloud(settings);
        
        // ------------------
        // Configure capture
        
        configureCapture(settings);
        
        // -------------------
        // Take first images

        for(ImageVector::iterator it = m_images.begin(); it != m_images.end(); it++)
        {
            delete *it;
        }

        m_images.clear();
        m_images = capture->takeImages(3);
        
        // --------------------
        // Initialize machinery

        if(machinery != 0) delete machinery;
        machinery = new Machinery();
        machinery->setup(settings);
        machinery->initialize(m_images);
    }
    
    // ----------------------------------
    // Configure capture device + thread
    
    void Kerberos::configureCapture(StringMap & settings)
    {
        // ---------------------------
        // Initialize capture device
        
        pthread_mutex_lock(&m_streamLock);
        if(capture != 0)
        {
            capture->stopGrabThread();
            capture->close();
            delete capture;
        }
        capture = Factory<Capture>::getInstance()->create(settings.at("capture"));
        capture->setup(settings);
        capture->startGrabThread();
        pthread_mutex_unlock(&m_streamLock);
    }
    
    // ----------------------------------
    // Configure cloud device + thread

    void Kerberos::configureCloud(StringMap & settings)
    {
        // ---------------------------
        // Initialize cloud service
        pthread_mutex_lock(&m_cloudLock);
        if(cloud != 0)
        {
            cloud->stopWatchThread();
            cloud->stopUploadThread();
            delete cloud;
        }
        
        cloud = Factory<Cloud>::getInstance()->create(settings.at("cloud"));
        pthread_mutex_unlock(&m_cloudLock);
        cloud->setLock(m_cloudLock);
        cloud->setup(settings);
    }

    // --------------------------------------------
    // Function ran in a thread, which continuously
    // stream MJPEG's.

    void * streamContinuously(void * self)
    {
        Kerberos * kerberos = (Kerberos *) self;

        while(kerberos->stream->isOpened())
        {
            try
            {
                pthread_mutex_lock(&kerberos->m_streamLock);
                kerberos->stream->connect();

                Image image = kerberos->capture->retrieve();
                if(kerberos->capture->m_angle != 0)
                {
                    image.rotate(kerberos->capture->m_angle);
                }
                kerberos->stream->write(image);

                pthread_mutex_unlock(&kerberos->m_streamLock);
                usleep(800*100);
            }
            catch(cv::Exception & ex)
            {
                pthread_mutex_unlock(&kerberos->m_streamLock);
            }
        }
    }
    
    void Kerberos::startStreamThread()
    {
        // ------------------------------------------------
        // Start a new thread that streams MJPEG's continuously.
        
        if(stream == 0)
        {
            stream = new Stream(8888);
        }
        
        pthread_create(&m_streamThread, NULL, streamContinuously, this);
    }
    
    void Kerberos::stopStreamThread()
    {
        // ----------------------------------
        // Cancel the existing stream thread,
        
        pthread_cancel(m_streamThread);
        pthread_join(m_streamThread, NULL);
    }

    // -------------------------------------------
    // Function ran in a thread, which continuously
    // checks if some detections occurred and
    // execute the IO devices if so.

    void * checkDetectionsContinuously(void * self)
    {
        Kerberos * kerberos = (Kerberos *) self;

        while(true)
        {
            int previousCount = 0;
            int currentCount = 0;

            try
            {
                previousCount = currentCount;
                currentCount = kerberos->m_detections.size();

                // If no new detections are found, we will run the IO devices
                if(previousCount == currentCount)
                {
                    pthread_mutex_lock(&kerberos->m_ioLock);
                    pthread_mutex_lock(&kerberos->m_cloudLock);

                    for (int i = 0; i < currentCount; i++)
                    {
                        Detection detection = kerberos->m_detections[i];
                        JSON data;
                        data.Parse(detection.t.c_str());

                        if(kerberos->machinery->save(detection.k, data))
                        {
                            kerberos->m_detections.erase(kerberos->m_detections.begin() + i);
                        }
                    }

                    pthread_mutex_unlock(&kerberos->m_cloudLock);
                    pthread_mutex_unlock(&kerberos->m_ioLock);
                }

                usleep(3000*100);
            }
            catch(cv::Exception & ex)
            {
                pthread_mutex_unlock(&kerberos->m_ioLock);
                pthread_mutex_unlock(&kerberos->m_cloudLock);
            }
        }
    }

    void Kerberos::startIOThread()
    {
        // ------------------------------------------------
        // Start a new thread that cheks for detections

        pthread_create(&m_ioThread, NULL, checkDetectionsContinuously, this);
    }

    void Kerberos::stopIOThread()
    {
        // ----------------------------------
        // Cancel the existing io thread,

        pthread_cancel(m_ioThread);
        pthread_join(m_ioThread, NULL);
    }
}