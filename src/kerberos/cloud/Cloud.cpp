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
        setName(settings.at("name"));
        setCapture(settings.at("capture"));
        setCaptureDirectory(settings.at("ios.Video.directory"));
        generateHash(settings);

        std::string homePath;
        if(System::isKiOS() == "true")
        {
            homePath = "/data/machinery";
        }
        else if(getenv("HOME") != 0)
        {
            homePath = (std::string) getenv("HOME");
        }
        else
        {
            homePath = "/var/tmp";
        }

        m_keyFile = homePath + "/kerberosio.key";

        std::ifstream keyFile(m_keyFile);
        if(keyFile.is_open())
        {
            std::string key;
            while(getline(keyFile,key))
            {
                setProductKey(key);
            }
            keyFile.close();
        }
        else
        {
            setProductKey("");
        }

        startPollThread();
        startUploadThread();

        // -------------
        // Health thread

        std::string user = settings.at("clouds.S3.folder");
        std::string publicKey = settings.at("clouds.S3.publicKey");
        std::string privateKey = settings.at("clouds.S3.privateKey");
        setCloudCredentials(user, publicKey, privateKey);
        startHealthThread();
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

                std::vector<std::string> fileParts;
                helper::tokenize(file, fileParts, ".");
                if(fileParts[1] == "h264")
                {
                    it++;
                    continue;
                }

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
        Cloud * cloud = (Cloud *) clo;

        // ----------------------
        // Create connection object

        RestClient::Connection * conn = cloud->pollConnection;
        conn->SetTimeout(5); // set connection timeout to 5s

        RestClient::HeaderFields headers;
        headers["Content-Type"] = "application/json";
        conn->SetHeaders(headers);

        std::string raspberrypi = (RUNNING_ON_A_RASPBERRYPI ? "true" : "false");

        std::string version = "{";
        version += "\"version\": \"" + (std::string) VERSION + "\",";
        version += "\"docker\": " + System::isDocker() + ",";
        version += "\"kios\": " + System::isKiOS() + ",";
        version += "\"raspberrypi\": " + raspberrypi;
        version += "}";

        while(true)
        {
            RestClient::Response r = conn->post("/", version);
            usleep(180*1000*1000);
        }
    }

    void Cloud::startPollThread()
    {
        pollConnection = new RestClient::Connection(HADES);
        pthread_create(&m_pollThread, NULL, pollContinuously, this);
    }

    void Cloud::stopPollThread()
    {
        delete pollConnection;
        pthread_cancel(m_pollThread);
        pthread_join(m_pollThread, NULL);
    }

    // ------------------------------------
    // Send device information to cloud, if
    // subscribed to our cloud plan.

    void * deviceHealth(void * clo)
    {
        Cloud * cloud = (Cloud *) clo;

        // ----------------------
        // Check if a cloud user

        if(cloud->m_user != "" &&
           cloud->m_publicKey != ""&&
           cloud->m_privateKey != ""
        )
        {
            // ----------------------
            // Create connection object

            RestClient::Connection * conn = cloud->cloudConnection;
            //conn->SetTimeout(5); // set connection timeout to 5s

            RestClient::HeaderFields headers;
            headers["Content-Type"] = "application/json";
            conn->SetHeaders(headers);

            // -----------------------------------
            // Check if we need to generate a
            // new product key (first time running)

            if(cloud->m_productKey == "")
            {
                // Generate random key
                std::string key = helper::random_string(26);

                // Create product key file
                std::string command = "touch " + cloud->m_keyFile;
                std::string createProductKeyFile = helper::GetStdoutFromCommand(command);
                BINFO << "Cloud: create key file";

                // Write product key
                command = "echo " + key + " > " + cloud->m_keyFile;
                std::string writeProductKey = helper::GetStdoutFromCommand(command);
                BINFO << "Cloud: write key";

                // Reset key
                cloud->setProductKey(key);

                BINFO << "Cloud: reset product key (" << key << ")";
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

                RestClient::Response r = conn->post("/api/v1/health", health);
                BINFO << "Cloud: data - " << health;
                BINFO << "Cloud: send device health - " << r.code;
                BINFO << "Cloud: send device health - " << r.body;
                usleep(15*1000*1000); // every 15s
            }
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
        cloudConnection = new RestClient::Connection(CLOUD);
        pthread_create(&m_healthThread, NULL, deviceHealth, this);
        pthread_detach(m_healthThread);
    }

    void Cloud::stopHealthThread()
    {
        delete cloudConnection;
        pthread_cancel(m_healthThread);
        pthread_join(m_healthThread, NULL);
    }
}
