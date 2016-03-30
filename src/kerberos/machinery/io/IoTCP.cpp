#include "machinery/io/IoTCP.h"

namespace kerberos
{
    void IoTCP::setup(const StringMap & settings)
    {
        Io::setup(settings);

        // ----------------------------------
        // TODO: set server credentials
        
        setIp(settings.at("ios.TCPSocket.server").c_str());
        setPort(std::atoi(settings.at("ios.TCPSocket.port").c_str()));
        setMessage(settings.at("ios.TCPSocket.message").c_str());
    }
    
    bool IoTCP::save(Image & image, JSON & data)
    {
        int sock;
        struct sockaddr_in echoServAddr;
        
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) >= 0)
        {
            memset(&echoServAddr, 0, sizeof(echoServAddr));
            echoServAddr.sin_family = AF_INET;
            echoServAddr.sin_addr.s_addr = inet_addr(getIp());
            echoServAddr.sin_port = htons(getPort());
            
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000;
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
            
            if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) >= 0)
            {
                BINFO << "IoTCP: sending TCP packet to " + (std::string) getIp() + ":" + helper::to_string(getPort());
                if (send(sock, getMessage(), strlen(getMessage()), 0) != strlen(getMessage()))
                {
                    throw new SocketException("send() sent a different number of bytes than expected");
                }
                
                close(sock);
            }
        }
        
        return true;
    }
}