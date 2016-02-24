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
        
        stream = new Stream(8888);
        startStreamThread();
        
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

            JSON m_data;
            m_data.SetObject();

            // ------------------------------------
            // Guard look if the configuration has
            // been changed...
            
            guard->look();
            
            // --------------------------------------------
            // If machinery is NOT allowed to do detection
            // continue iteration
            
            if(!machinery->allowed(images))
            {
                continue;
            }
            
            // --------------------
            // Clean image to save

            Image cleanImage = *images[images.size()-1];

            // --------------
            // Processing..
            
            if(machinery->detect(images, m_data))
            {
                pthread_mutex_lock(&m_cloudLock);
                machinery->save(cleanImage, m_data);
                pthread_mutex_unlock(&m_cloudLock);
            }

            // -------------
            // Shift images
            
            images = capture->shiftImage();
            usleep(1000*1000);
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

        for(ImageVector::iterator it = images.begin(); it != images.end(); it++)
        {
            delete *it;
        }

        images.clear();
        images = capture->takeImages(3);
        
        // --------------------
        // Initialize machinery

        if(machinery != 0) delete machinery;
        machinery = new Machinery();
        machinery->setup(settings);
        machinery->initialize(images);
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
    
    // -------------------------------------------
    // Function ran in a thread, which continously
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
                kerberos->stream->write(kerberos->capture->retrieve());
                pthread_mutex_unlock(&kerberos->m_streamLock);
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
        // Start a new thread that streams MJPEG's continously.
        
        pthread_create(&m_streamThread, NULL, streamContinuously, this);   
    }
    
    void Kerberos::stopStreamThread()
    {
        // ----------------------------------
        // Cancel the existing stream thread,
        
        pthread_detach(m_streamThread);
        pthread_cancel(m_streamThread);  
    }
}