#include "cloud/Cloud.h"

namespace kerberos
{
    void Cloud::setup(kerberos::StringMap & settings)
    {
        // -------------------------------
        // Upload interval [1.5sec;4.2min]
        
        m_min = 1500;
        m_max = 256000;
        m_interval = m_min;
        
        m_watchDirectory = settings.at("ios.Disk.directory");

        startPollThread();
        startUploadThread();
        startWatchThread();
    }
    
    void Cloud::addFile(const std::string & file)
    {
        std::ifstream f(file.c_str());
        if(f.good())
        {
            // Get filename
            std::vector<std::string> tokens;
            helper::tokenize(file, tokens, "/");
            std::string fileName = tokens[tokens.size()-1];

            // create a symbol link
            std::string link = SYMBOL_DIRECTORY + fileName;
            int beenCreated = symlink(file.c_str(), link.c_str());
            
            f.close();
        }
    }
    
    void Cloud::watch()
    {
        while(true)
        {
            try
            {
                guard = new FW::Guard();
                guard->listenTo(m_watchDirectory);
                guard->onChange(&Watcher::addFile);
                guard->startLookingForNewFiles();
        
                while(true)
                {
                    pthread_mutex_lock(&m_cloudLock);
                    guard->look();
                    pthread_mutex_unlock(&m_cloudLock);
                    usleep(2500*1000);
                }
            }
            catch(FW::FileNotFoundException & ex){}
            usleep(1000*1000);
        }
    }
    
    void Cloud::scan()
    {
        
        for(;;)
        {
            std::vector<std::string> storage;
            
            pthread_mutex_lock(&m_cloudLock);
            usleep(m_interval*1000);
            helper::getFilesInDirectory(storage, SYMBOL_DIRECTORY); // get all symbol links of directory
            pthread_mutex_unlock(&m_cloudLock);
                
            std::vector<std::string>::iterator it = storage.begin();
            
            while(it != storage.end())
            { 
                std::string file = *it;
               
                struct stat st;
                if(stat(file.c_str(), &st) == -1)
                {
                    pthread_mutex_lock(&m_cloudLock);
                    unlink(file.c_str()); // remove symbol link
                    pthread_mutex_unlock(&m_cloudLock);
                    break;
                }
                
                bool hasBeenUploaded = upload(file);
                if(hasBeenUploaded)
                {
                    pthread_mutex_lock(&m_cloudLock);
                    unlink(file.c_str()); // remove symbol link
                    pthread_mutex_unlock(&m_cloudLock);
                    m_interval = m_min; // reset interval
                }
                else
                {   
                    m_interval = (m_interval * 2 < m_max) ? m_interval * 2 : m_max; // update interval exponential.
                    break;
                }
                
                it++;
            }
        }
    }
    
    // --------------
    // Upload thread
    
    void * uploadContinuously(void * self)
    {
        Cloud * cloud = (Cloud *) self;
        cloud->scan();
    }
    
    void Cloud::startUploadThread()
    {
        pthread_create(&m_uploadThread, NULL, uploadContinuously, this);   
    }
    
    void Cloud::stopUploadThread()
    {
        pthread_mutex_unlock(&m_cloudLock);
        pthread_cancel(m_uploadThread);
        pthread_join(m_uploadThread, NULL);
    }
    
    // --------------
    // Watch thread
    
    void * watchContinuously(void * self)
    {
        Cloud * cloud = (Cloud *) self;
        cloud->watch();
    }
    
    void Cloud::startWatchThread()
    {
        if(m_watchDirectory != "")
        { 
            pthread_create(&m_watchThread, NULL, watchContinuously, this);
        }
    }
    
    void Cloud::stopWatchThread()
    {
        pthread_mutex_unlock(&m_cloudLock);
        pthread_cancel(m_watchThread);
        pthread_join(m_watchThread, NULL);
    }

    // --------------
    // Poll hades

    void * pollContinuously(void * clo)
    {
        std::string version = "{\"version\": \"";
        version += VERSION;
        version += "\"}";

        while(true)
        {
            RestClient::post(HADES, "application/json", version);
            usleep(180*1000*1000);
        }
    }

    void Cloud::startPollThread()
    {
        pthread_create(&m_pollThread, NULL, pollContinuously, this);
    }

    void Cloud::stopPollThread()
    {
        pthread_cancel(m_pollThread);
        pthread_join(m_pollThread, NULL);
    }
}