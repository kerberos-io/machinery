//
//  Class: IoTCP
//  Description: A TCP socket that sends a packet
//               to a server.
//
//  Created:     18/12/2014
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

#ifndef __IoTCP_H_INCLUDED__   // if IoTCP.h hasn't been included yet...
#define __IoTCP_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"
#include "Exception.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace kerberos
{
    char TCPName[] = "TCPSocket";
    class IoTCP : public IoCreator<TCPName, IoTCP>
    {
        private:
            std::string m_server_ip;
            unsigned short m_port;
            std::string m_message;
        
        public:
            IoTCP(){};
            void setup(const StringMap & settings);
            void fire(JSON & data){};
            void disableCapture(){};
        
            void setIp(const std::string server_ip){m_server_ip=server_ip;};
            const char * getIp(){return m_server_ip.c_str();};
            void setPort(const unsigned short port){m_port=port;};
            unsigned short getPort(){return m_port;};
            void setMessage(std::string message){m_message=message;};
            const char * getMessage(){return m_message.c_str();};
        
            bool save(Image & image){ return true; };
            bool save(Image & image, JSON & data);
    };
}
#endif