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
                machinery->save(cleanImage, m_data);
            }

            // -------------
            // Shift images
            
            images = capture->shiftImage();
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
        
        if(capture != 0)
        {
            capture->stopGrabThread();
            capture->close();
            delete capture;
        }
        capture = Factory<Capture>::getInstance()->create(settings.at("capture"));
        capture->setup(settings);
        
        capture->startGrabThread();
    }
    
    // ----------------------------------
    // Configure cloud device + thread

    void Kerberos::configureCloud(StringMap & settings)
    {
        // ---------------------------
        // Initialize cloud service
        
        if(cloud != 0)
        {
            cloud->stopWatchThread();
            cloud->stopUploadThread();
            delete cloud;
        }
        
        cloud = Factory<Cloud>::getInstance()->create(settings.at("cloud"));
        cloud->setup(settings);
    }
}