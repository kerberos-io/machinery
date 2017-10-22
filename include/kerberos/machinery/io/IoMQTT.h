//
//  Class: IoMQTT
//  Description:  Send message to MQTT topic 
//  Created:     20/10/2017
//  Author:      Gianrico D'Angelis
//  Mail:        gianrico.dangelis@gmail.com
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied under
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#ifndef __IoMQTT_H_INCLUDED__   // if IoMQTT.h hasn't been included yet...
#define __IoMQTT_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"
#include <mosquittopp.h>
#include "writer.h"
#include "Throttler.h"

namespace kerberos
{
    char MQTTName[] = "MQTT";
    class IoMQTT : public IoCreator<MQTTName, IoMQTT>, public mosqpp::mosquittopp 
    {
        private:
	    	std::string m_server_ip;
		unsigned short m_port;
	        std::string m_topic;
		
            	Throttler throttle;

        public:
            IoMQTT(){};
            virtual ~IoMQTT()
            {
		disconnect();
		loop_stop();   
		mosqpp::lib_cleanup();
            };

            void setup(const StringMap & settings);

            void disableCapture(){};


            void setIp(const std::string server_ip){m_server_ip=server_ip;};
	    const char * getIp(){return m_server_ip.c_str();};
	    void setPort(const unsigned short port){m_port=port;};
	    unsigned short getPort(){return m_port;};
	    void setTopic(std::string topic){m_topic=topic;};
	    const char * getTopic(){return m_topic.c_str();};

	    bool send_message(std::string &message);

            bool save(Image & image); 
            bool save(Image & image, JSON & data); 

            void fire(JSON & data){};
    };
}
#endif
