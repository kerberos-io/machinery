#include "cloud/Cloud.h"

namespace kerberos
{
    void Cloud::setup(kerberos::StringMap & settings)
    {
        // -------------------------------
        // Upload interval [0.5sec;4.2min]
        
        m_min = 1500;
        m_max = 256000;
        m_interval = m_min;
        
        startUploadThread();
        startWatchThread(settings);
    }
    
    void Cloud::scan()
    {
        
        for(;;)
        {
            usleep(m_min*1000);

            std::vector<std::string> storage;
            
            pthread_mutex_lock(&m_cloudLock);
            helper::getFilesInDirectory(storage, SYMBOL_DIRECTORY); // get all symbol links of directory
            pthread_mutex_unlock(&m_cloudLock);
                
            std::vector<std::string>::iterator it = storage.begin();
            
            while(it != storage.end())
            { 
                std::string file = *it;
                bool hasBeenUploaded = upload(file);
                
                if(hasBeenUploaded)
                {
                    unlink(file.c_str()); // remove symbol link
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
    
    void * uploadContinuously(void * clo)
    {
        Cloud * cloud = (Cloud *) clo;
        cloud->scan();
    }
    
    void Cloud::startUploadThread()
    {
        pthread_create(&m_uploadThread, NULL, uploadContinuously, this);   
    }
    
    void Cloud::stopUploadThread()
    {
        pthread_detach(m_uploadThread);
        pthread_cancel(m_uploadThread);  
    }
    
    // --------------
    // Watch thread
    
    void * watchContinuously(void * file)
    {
        char * fileDirectory = (char *) file;
        Watcher watch;
        watch.setup(fileDirectory);
        watch.scan();
    }
    
    void Cloud::startWatchThread(StringMap & settings)
    {
        const char * file = settings.at("ios.Disk.directory").c_str();
        if(file != 0)
        {
            pthread_create(&m_watchThread, NULL, watchContinuously, (char *) file);
        }
    }
    
    void Cloud::stopWatchThread()
    {
        pthread_detach(m_watchThread);
        pthread_cancel(m_watchThread);  
    }
}