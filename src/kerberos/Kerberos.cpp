#include "Kerberos.h"

namespace kerberos
{
    void Kerberos::bootstrap(StringMap & parameters)
    {
        // --------------------------------
        // Set parameters from command-line
        
        setParameters(parameters);
        
        // ---------------------
        // Initialize kerberos
        
        std::string configuration = (helper::getValueByKey(parameters, "config")) ?: CONFIGURATION_PATH;
        configure(configuration);

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
                BINFO << "Machinery on hold, conditions failed.";
                continue;
            }
            
            // --------------------
            // Clean image to save

            Image cleanImage = *m_images[m_images.size()-1];

            // --------------
            // Processing..
            
            if(machinery->detect(m_images, data))
            {
                // ---------------------------
                // If something is detected...
                
                pthread_mutex_lock(&m_ioLock);

                Detection detection(toJSON(data), cleanImage);
                m_detections.push_back(detection);

                pthread_mutex_unlock(&m_ioLock);
            }

            // -------------
            // Shift images

            m_images = capture->shiftImage();
        }
    }

    std::string Kerberos::toJSON(JSON & data)
    {
        JSON::AllocatorType& allocator = data.GetAllocator();

        JSONValue timestamp;
        timestamp.SetString(kerberos::helper::getTimestamp().c_str(), allocator);
        data.AddMember("timestamp", timestamp, allocator);

        std::string micro = kerberos::helper::getMicroseconds();
        micro = kerberos::helper::to_string((int)micro.length()) + "-" + micro;

        JSONValue microseconds;
        microseconds.SetString(micro.c_str(), allocator);
        data.AddMember("microseconds", microseconds, allocator);

        data.AddMember("token", rand()%1000, allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        data.Accept(writer);
        return buffer.GetString();
    }

    void Kerberos::configure(const std::string & configuration)
    {
        // ---------------------------
    	// Get settings from XML file
        
        LINFO << "Reading configuration file: " << configuration;
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
        
        LINFO << helper::printStringMap("Final configuration:", settings);

        // -------------------------------------------
        // Check if we need to disable verbose logging

        
        easyloggingpp::Logger * logger = easyloggingpp::Loggers::getLogger("business");
        easyloggingpp::Configurations & config = logger->configurations();
        if(settings.at("logging") == "false")
        {
            LINFO << "Logging is set to info";
            config.set(easyloggingpp::Level::Info, easyloggingpp::ConfigurationType::Enabled, "false");
        }
        else
        {
            LINFO << "Logging is set to verbose";
            config.set(easyloggingpp::Level::Info, easyloggingpp::ConfigurationType::Enabled, "true");
        }
        logger->reconfigure();

        // -----------------
        // Configure cloud
        
        configureCloud(settings);
        
        // ------------------
        // Configure capture
        
        configureCapture(settings);
        
        // --------------------
        // Initialize machinery

        if(machinery != 0) delete machinery;
        machinery = new Machinery();
        machinery->setCapture(capture);
        machinery->setup(settings);
        
        // -------------------
        // Take first images
        
        for(ImageVector::iterator it = m_images.begin(); it != m_images.end(); it++)
        {
            delete *it;
        }
        
        m_images.clear();
        m_images = capture->takeImages(3);
        
        machinery->initialize(m_images);
    }
    
    // ----------------------------------
    // Configure capture device + stream
    
    void Kerberos::configureCapture(StringMap & settings)
    {
        // -----------------------
        // Stop stream and capture
        
        if(stream != 0)
        {
            LINFO << "Stopping streaming";
            stopStreamThread();
            delete stream;
            stream = 0;
        }
        
        if(capture != 0)
        {
            LINFO << "Stopping capture device";
            if(capture->isOpened())
            {
                machinery->disableCapture();
                capture->stopGrabThread();
                capture->close();
            }
            delete capture;
            capture = 0;
        }
        
        // ---------------------------
        // Initialize capture device
        
        LINFO << "Starting capture device: " + settings.at("capture");
        capture = Factory<Capture>::getInstance()->create(settings.at("capture"));
        capture->setup(settings);
        capture->startGrabThread();
        
        // ------------------
        // Initialize stream
        
        stream = new Stream();
        stream->configureStream(settings);
        startStreamThread();
    }
    
    // ----------------------------------
    // Configure cloud device + thread

    void Kerberos::configureCloud(StringMap & settings)
    {
        // ---------------------------
        // Initialize cloud service
        
        if(cloud != 0)
        {
            LINFO << "Stopping cloud service";
            cloud->stopUploadThread();
            cloud->stopPollThread();
            delete cloud;
        }

        LINFO << "Starting cloud service: " + settings.at("cloud");
        cloud = Factory<Cloud>::getInstance()->create(settings.at("cloud"));
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
                kerberos->stream->connect();
                
                if(kerberos->capture->isOpened())
                {
                    Image image = kerberos->capture->retrieve();
                    if(kerberos->capture->m_angle != 0)
                    {
                        image.rotate(kerberos->capture->m_angle);
                    }
                    kerberos->stream->write(image);
                }
                
                usleep(kerberos->stream->wait * 1000 * 1000); // sleep x microsec.
            }
            catch(cv::Exception & ex){}
        }
    }
    
    void Kerberos::startStreamThread()
    {
        // ------------------------------------------------
        // Start a new thread that streams MJPEG's continuously.
        
        if(stream != 0)
        {
            //if stream object just exists try to open configured stream port
            stream->open();
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

        int previousCount = 0;
        int currentCount = 0;
        int timesEqual = 0;

        while(true)
        {
            try
            {
                previousCount = currentCount;
                currentCount = kerberos->m_detections.size();

                if(previousCount == currentCount)
                {
                    timesEqual++;
                }
                else if(timesEqual > 0)
                {
                    timesEqual--;
                }

                // If no new detections are found, we will run the IO devices (or max 30 images in memory)
                if((currentCount > 0 && timesEqual > 4) || currentCount >= 30)
                {
                    BINFO << "Executing IO devices for " + helper::to_string(currentCount)  + " detection(s)";

                    for (int i = 0; i < currentCount; i++)
                    {
                        Detection detection = kerberos->m_detections[0];
                        JSON data;
                        data.Parse(detection.t.c_str());

                        pthread_mutex_lock(&kerberos->m_ioLock);
                        if(kerberos->machinery->save(detection.k, data))
                        {
                            kerberos->m_detections.erase(kerberos->m_detections.begin());
                        }
                        else
                        {
                            LERROR << "IO: can't execute";
                        }
                        pthread_mutex_unlock(&kerberos->m_ioLock);
                        usleep(500*1000);
                    }

                    timesEqual = 0;
                }

                usleep(500*1000);
            }
            catch(cv::Exception & ex)
            {
                pthread_mutex_unlock(&kerberos->m_ioLock);
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