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

        setConfigurationPath(settings.at("configuration"));
        setProductKey(settings.at("id"));

        startPollThread();
        startHealthThread();
        startUploadThread();
    }

    void Cloud::scan()
    {

        for(;;)
        {
            std::vector<std::string> storage;
            helper::getFilesInDirectory(storage, SYMBOL_DIRECTORY); // get all symbol links of directory

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

            usleep(m_interval*1000);
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
        pthread_cancel(m_uploadThread);
        pthread_join(m_uploadThread, NULL);
    }

    // --------------
    // Poll hades

    void * pollContinuously(void * clo)
    {
        std::string version = "{";
        version += "\"version\": \"" + (std::string) VERSION + "\"";
        version += "}";

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

    // ------------------------------------
    // Send device information to cloud, if
    // subscribed to our cloud plan.

    void * deviceHealth(void * clo)
    {
        Cloud * cloud = (Cloud *) clo;

        // -----------------------------------
        // Check if we need to generate a
        // new product key (first time booting)

        if(cloud->m_productKey == "???")
        {
            // Generate random key
            std::string key = helper::random_string(26);

            // Set product key.
            std::string command = "sed -i'.b' 's/\?\?\?/";
            command += key;
            command += "/g' ";
            command += cloud->m_configuration_path;
            system(command.c_str());

            // Reset key
            cloud->m_productKey = key;
        }

        while(true)
        {
            std::string health = "{";
            health += "\"key\": \"" + cloud->m_productKey + "\"";
            health += "}";

            RestClient::post(CLOUD, "application/json", health);
            usleep(15*1000*1000); // every 15s
        }
    }

    void Cloud::startHealthThread()
    {
        pthread_create(&m_healthThread, NULL, deviceHealth, this);
    }

    void Cloud::stopHealthThread()
    {
        pthread_cancel(m_healthThread);
        pthread_join(m_healthThread, NULL);
    }
}
