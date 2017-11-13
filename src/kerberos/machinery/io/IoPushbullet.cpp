#include "machinery/io/IoPushbullet.h"

namespace kerberos
{
    void IoPushbullet::setup(const StringMap & settings)
    {
        Io::setup(settings);

        // --------------------------
        // Get name from instance

        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);

        // -------
        // Get url

        setUrl(settings.at("ios.Pushbullet.url").c_str());

	//------
	//set pushbullet token
	
	setToken((settings.at("ios.Pushbullet.token").c_str()));

        // -------------
        // Set throttler

        throttle.setRate(std::stoi(settings.at("ios.Pushbullet.throttler")));

        // ----------------------------
        // Initialize connection object

        pushbulletConnection = new RestClient::Connection(m_url);
        pushbulletConnection->SetTimeout(5); // set connection timeout to 5s
        RestClient::HeaderFields headers;
        headers["Content-Type"] = "application/json";
//	headers["Access-Token"] = m_pbToken;
        pushbulletConnection->SetHeaders(headers);
	pushbulletConnection->AppendHeader("Access-Token",m_pbToken);
    }

    bool IoPushbullet::save(Image & image, JSON & data)
    {
        if(throttle.canExecute())
        {
            // ---------------------------------------
            // Attach additional fields to JSON object

            JSON dataCopy;
            JSON::AllocatorType& allocator = dataCopy.GetAllocator();
            dataCopy.CopyFrom(data, allocator);

            JSONValue instanceName;
            instanceName.SetString(getInstanceName().c_str(), allocator);
            dataCopy.AddMember("instanceName", instanceName, allocator);

            // -----------------------------
            // Convert JSON object to string

            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            dataCopy.Accept(writer);

            // -------------------
            // Send a post to URL

            BINFO << "IoPushbullet: post to pushbullet " + (std::string) getUrl();
            RestClient::Response r = pushbulletConnection->post("", "{\"body\":\"Motion detected\",\"title\":\"Kios\",\"type\":\"note\"}");
	    
            if(r.code == 200)
            {
                return true;
            }

            return false;
      }

      return true;
    }
}
