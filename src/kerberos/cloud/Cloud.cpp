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
        setCapture(settings.at("capture"));
        generateHash(settings);

        startPollThread();
        startUploadThread();

        std::string user = settings.at("clouds.S3.folder");
        std::string publicKey = settings.at("clouds.S3.publicKey");
        std::string privateKey = settings.at("clouds.S3.privateKey");
        setCloudCredentials(user, publicKey, privateKey);

        if(m_user != "" &&
           m_publicKey != "" &&
           m_privateKey != ""
        )
        {
            startHealthThread();
        }
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
        // new product key (first time running)

        if(cloud->m_productKey == "???")
        {
            // Generate random key
            std::string key = helper::random_string(26);

            // Set product key
            std::string command = "sed -i'.b' 's/\?\?\?/";
            command += key;
            command += "/g' ";
            command += cloud->m_configuration_path;
            system(command.c_str());

            // Remove backup file
            command = "rm " + cloud->m_configuration_path + ".b";
            system(command.c_str());

            // Reset key
            cloud->setProductKey(key);
        }

        // --------------------------------------------
        // Generate fixed JSON data which will be send,
        // over and over again.

        std:: string fixedProperties = "";
        fixedProperties += "\"key\": \"" + cloud->m_productKey + "\",";
        fixedProperties += "\"version\": \"" + (std::string) VERSION + "\",";
        fixedProperties += "\"hostname\": \"" + cloud->getHostname() + "\",";
        fixedProperties += "\"hash\": \"" + cloud->m_hash + "\",";
        fixedProperties += "\"capture\": \"" + cloud->m_capture + "\",";
        fixedProperties += "\"cloudUser\": \"" + cloud->m_user + "\",";
        fixedProperties += "\"cloudPublicKey\": \"" + cloud->m_publicKey + "\",";
        fixedProperties += "\"cloudPrivateKey\": \"" + cloud->m_privateKey + "\",";

        std::string raspberrypi = (RUNNING_ON_A_RASPBERRYPI ? "true" : "false");
        fixedProperties += "\"raspberrypi\": " + raspberrypi + ",";

        // ------------------------------------------
        // Send client data to the cloud application.

        std::string url = (std::string) CLOUD + "/api/v1/health";

        while(true)
        {
            std::string health = "{";
            health += fixedProperties;
            health += "\"disk1Size\": \"" + cloud->getDiskPercentage("1") + "\",";
            health += "\"disk3Size\": \"" + cloud->getDiskPercentage("3") + "\",";
            health += "\"temperature\": \"" + cloud->getTemperature() + "\",";
            health += "\"wifiSSID\": \"" + cloud->getWifiSSID() + "\",";
            health += "\"wifiStrength\": \"" + cloud->getWifiStrength() + "\"";
            health += "}";

            RestClient::post(url, "application/json", health);
            usleep(5*1000*1000); // every 5s
        }
    }

    void Cloud::generateHash(kerberos::StringMap & settings)
    {
        std::string values = settings.at("name");
        values += settings.at("capture");
        values += settings.at("captures.USBCamera.deviceNumber");
        values += settings.at("captures.IPCamera.url");
        values += settings.at("streams.Mjpg.streamPort");

        std::hash<std::string> hasher;
        m_hash = std::to_string(hasher(values));
    }

    void Cloud::setCloudCredentials(std::string user, std::string publicKey, std::string privateKey)
    {
        m_user = user;
        m_publicKey = publicKey;
        m_privateKey = privateKey;
    }

    std::string Cloud::getHostname()
    {
        std::string hostname = helper::GetStdoutFromCommand("hostname | tr '\n' ' '");
        return hostname;
    }

    std::string Cloud::getDiskPercentage(std::string partition)
    {
        std::string percentage = "-1";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            percentage = helper::GetStdoutFromCommand("echo $(df -h | grep /dev/mmcblk0p" + partition + " | head -1 | awk -F' ' '{ print $5/1 }' | tr ['%'] [\"0\"]) | tr '\n' ' '");
        }

        return percentage;
    }

    std::string Cloud::getTemperature()
    {
        std::string temperature = "-1";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            temperature = helper::GetStdoutFromCommand("vcgencmd measure_temp | tr '\n' ' '");
        }

        return temperature;
    }

    std::string Cloud::getWifiSSID()
    {
        std::string ssid = "-1";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            ssid = helper::GetStdoutFromCommand("iwconfig wlan0 | grep \"ESSID\" | tr '\n' ' '");
        }

        return ssid;
    }

    std::string Cloud::getWifiStrength()
    {
        std::string strength = "-1";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            strength = helper::GetStdoutFromCommand("iwconfig wlan0 | grep \"Link Quality\" | sed -e 's/^[ \t]*//' | tr '\n' ' '");
        }

        return strength;
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
