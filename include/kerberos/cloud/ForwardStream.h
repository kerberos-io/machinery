//
//  Class: ForwardStream
//  Description: ForwardStream
//
//  Created:     16/04/2018
//  Author:      Cédric Verstraeten
//  Mail:        cedric@verstraeten.io
//  Website:     www.verstraeten.io
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied under
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#ifndef __ForwardStream_H_INCLUDED__   // if ForwardStream.h hasn't been included yet...
#define __ForwardStream_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Exception.hpp"
#include "capture/Image.h"
#include "mosquittopp.h"
#include "writer.h"
#include "base64.h"
#include "Helper.h"
#include "machinery/io/Throttler.h"

namespace kerberos
{
    class ForwardStream : public mosqpp::mosquittopp
    {
        private:
            std::string m_server_ip;
            unsigned short m_port;
            std::string m_topic;
            std::string m_motion_topic;
            std::vector<int> m_encode_params;

        public:
            ForwardStream(){};
            virtual ~ForwardStream()
            {
                disconnect();
                loop_stop();
                mosqpp::lib_cleanup();
            };

            std::string m_publicKey;
            std::string m_deviceKey;
            int m_lastReceived;
            Throttler throttle;

            void setup(std::string publickey, std::string deviceKey);
            void setIp(const std::string server_ip){m_server_ip=server_ip;};
            const char * getIp(){return m_server_ip.c_str();};
            void setPort(const unsigned short port){m_port=port;};
            unsigned short getPort(){return m_port;};
            void setTopic(std::string topic){m_topic=topic;};
            const char * getTopic(){return m_topic.c_str();};
            void setMotionTopic(std::string motion_topic){m_motion_topic=motion_topic;};
            const char * getMotionTopic(){return m_motion_topic.c_str();};
            cv::Mat GetSquareImage(const cv::Mat& img, int target_width = 500);
            bool forward(Image & cleanImage);
            bool forwardRAW(uint8_t * data, int32_t length);
            bool triggerMotion();
            bool isRequestingLiveStream();

            void on_connect(int rc);
        		void on_message(const struct mosquitto_message *message);
        		void on_subscribe(int mid, int qos_count, const int *granted_qos);
    };
}
#endif
