#include "cloud/Cloud.h"
#include "System.h"

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
        setName(settings.at("name"));
        setCapture(settings.at("capture"));
        setCaptureDirectory(settings.at("ios.Video.directory"));
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
        fixedProperties += "\"hash\": \"" + cloud->m_hash + "\",";
        fixedProperties += "\"version\": \"" + (std::string) VERSION + "\",";
        fixedProperties += "\"cpuId\": \"" + System::getCPUid() + "\",";

        fixedProperties += "\"cloudUser\": \"" + cloud->m_user + "\",";
        fixedProperties += "\"cloudPublicKey\": \"" + cloud->m_publicKey + "\",";
        fixedProperties += "\"cloudPrivateKey\": \"" + cloud->m_privateKey + "\",";

        fixedProperties += "\"cameraName\": \"" + cloud->m_name + "\",";
        fixedProperties += "\"cameraType\": \"" + cloud->m_capture + "\",";

        fixedProperties += "\"hostname\": \"" + System::getHostname() + "\",";
        fixedProperties += "\"docker\": " + System::isDocker() + ",";
        fixedProperties += "\"kios\": " + System::isKiOS() + ",";
        std::string raspberrypi = (RUNNING_ON_A_RASPBERRYPI ? "true" : "false");
        fixedProperties += "\"raspberrypi\": " + raspberrypi + ",";
        fixedProperties += "\"board\": \"" + System::getBoard() + "\",";

        // ------------------------------------------
        // Send client data to the cloud application.

        std::string url = (std::string) CLOUD + "/api/v1/health";

        while(true)
        {
            std::string health = "{";
            health += fixedProperties;
            health += "\"disk1Size\": \"" + System::getDiskPercentage("mmcblk0p1") + "\",";
            health += "\"disk3Size\": \"" + System::getDiskPercentage("mmcblk0p3") + "\",";
            health += "\"diskVDASize\": \"" + System::getDiskPercentage("vda1") + "\",";
            health += "\"numberOfFiles\": \"" + System::getNumberOfFiles(cloud->m_directory) + "\",";
            health += "\"temperature\": \"" + System::getTemperature() + "\",";
            health += "\"wifiSSID\": \"" + System::getWifiSSID() + "\",";
            health += "\"wifiStrength\": \"" + System::getWifiStrength() + "\",";
            health += "\"uptime\": \"" + System::getUptime() + "\"";
            health += "}";

            LINFO << health;

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
