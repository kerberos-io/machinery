#include "machinery/io/IoMQTT.h"

namespace kerberos
{
    void IoMQTT::setup(const StringMap & settings)
    {
        Io::setup(settings);
	BINFO << "IoMQTT setup";

        throttle.setRate(std::stoi(settings.at("ios.MQTT.throttler")));
        setIp(settings.at("ios.MQTT.server").c_str());
        setPort(std::atoi(settings.at("ios.MQTT.port").c_str()));
        setTopic(settings.at("ios.MQTT.topic").c_str());

	reinitialise(settings.at("name").c_str(),true);

	mosqpp::lib_init();
	connect_async(m_server_ip.c_str(),m_port);
	loop_start();


    }

    bool IoMQTT::save(Image & image)
    { 
	    BINFO << "IOMQTT save"; 
	    return true; 
    } 

    bool IoMQTT::save(Image & image, JSON & data)
    {
        if(throttle.canExecute())
        {
           JSON dataCopy;
	   JSON::AllocatorType& allocator = dataCopy.GetAllocator();
	   dataCopy.CopyFrom(data, allocator);

	   rapidjson::StringBuffer buffer;
	   rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	   dataCopy.Accept(writer);

	   std::string message { buffer.GetString() };
	  
	   BINFO << "IoMQTT: sending message..."<<((send_message(message))?"sent":"error");
           return true;
        }

        return true;
    }

    bool IoMQTT::send_message(std::string &_message)
    {
	 int ret = publish(NULL,m_topic.c_str(),_message.size(),_message.c_str(),1,false);
         return ( ret == MOSQ_ERR_SUCCESS );
    }
}
