#include "machinery/io/IoMQTT.h"

namespace kerberos
{
    void IoMQTT::setup(const StringMap & settings)
    {
        Io::setup(settings);

        throttle.setRate(std::stoi(settings.at("ios.MQTT.throttler")));
        setIp(settings.at("ios.MQTT.server").c_str());
        setPort(std::atoi(settings.at("ios.MQTT.port").c_str()));
        setTopic(settings.at("ios.MQTT.topic").c_str());
        setUsername(settings.at("ios.MQTT.username"));
        setPassword(settings.at("ios.MQTT.password"));
        setSecure(settings.at("ios.MQTT.secure")=="true");
        setVerifycn(settings.at("ios.MQTT.verifycn")=="true");

	      reinitialise(settings.at("name").c_str(),true);

        mosqpp::lib_init();
	      if(m_username.length()==0)
        {
		        username_pw_set(nullptr,nullptr);
        }
	      else
        {
		        username_pw_set(m_username.c_str(),m_password.c_str());
        }

        if(!m_verifycn)
        {
		        tls_insecure_set(true);
        }

        if(m_secure)
        {
		        tls_set(nullptr,"/etc/ssl/certs/",nullptr,nullptr,nullptr);
        }

        connect_async(m_server_ip.c_str(),m_port);
      	loop_start();
    }

    bool IoMQTT::save(Image & image)
    {
	      return true;
    }

    bool IoMQTT::save(Image & image, JSON & data)
    {
        if(throttle.canExecute())
        {
            JSON::AllocatorType& allocator = data.GetAllocator();

            // -----------------------------------------------
            // It's possible to encode the image directly, and
            // send it over to the MQTT broker.

            //JSONValue path;
            //std::string imageEncoded = base64_encode(image.getImage().data, image.getColumns() * image.getRows());
            //path.SetString(imageEncoded.c_str(), allocator);
            //data.AddMember("imageEncoded", path, allocator);

            JSON dataCopy;
        	  dataCopy.CopyFrom(data, allocator);

        	  rapidjson::StringBuffer buffer;
        	  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        	  dataCopy.Accept(writer);

        	  std::string message { buffer.GetString() };

            bool sendSuccessfully = send_message(message);
        	  BINFO << "IoMQTT: sending message..." << ((sendSuccessfully) ? "sent" : "error");

            return sendSuccessfully;
        }

        return true;
    }

    bool IoMQTT::send_message(std::string &_message)
    {
	      int ret = publish(NULL,m_topic.c_str(),_message.size(),_message.c_str(),1,false);
        return (ret == MOSQ_ERR_SUCCESS);
    }
}
