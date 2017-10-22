#include "machinery/io/IoWebhook.h"

namespace kerberos
{
    void IoWebhook::setup(const StringMap & settings)
    {
        Io::setup(settings);

        // --------------------------
        // Get name from instance

        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);

        // -------
        // Get url

        setUrl(settings.at("ios.Webhook.url").c_str());

        // -------------
        // Set throttler

        throttle.setRate(std::stoi(settings.at("ios.Webhook.throttler")));

        // ----------------------------
        // Initialize connection object

        webhookConnection = new RestClient::Connection(m_url);
        webhookConnection->SetTimeout(5); // set connection timeout to 5s
        RestClient::HeaderFields headers;
        headers["Content-Type"] = "application/json";
        webhookConnection->SetHeaders(headers);
    }

    bool IoWebhook::save(Image & image, JSON & data)
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

            BINFO << "IoWebhook: post to webhook " + (std::string) getUrl();
            RestClient::Response r = webhookConnection->post("/", buffer.GetString());

            if(r.code == 200)
            {
                return true;
            }

            return false;
      }

      return true;
    }
}
