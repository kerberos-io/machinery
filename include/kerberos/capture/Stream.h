//
//  Class: Stream
//  Description: Streaming images from Capture Devices as MJPEG.
//  Created:     15/02/2015
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

#include "Factory.h"
#include "capture/Image.h"

#ifndef __Stream_H_INCLUDED__   // if Stream.h hasn't been included yet...
#define __Stream_H_INCLUDED__   // #define this so the compiler knows it has been included

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#define PORT        unsigned short
#define SOCKET    int
#define HOSTENT  struct hostent
#define SOCKADDR    struct sockaddr
#define SOCKADDR_IN  struct sockaddr_in
#define ADDRPOINTER  unsigned int*
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

namespace kerberos
{
    class Stream
    {
        std::map<int, int> packetsSend;
        std::vector<SOCKET> clients;
        SOCKET sock;
        fd_set master;
        bool m_enabled;
        int m_streamPort;
        int m_timeout; // master sock timeout, shutdown after timeout millis.
        int m_quality; // jpeg compression [1..100]

        int _write( int sock, char *s, int len ) 
        { 
            if ( len < 1 ) { len = strlen(s); }
            #if defined(__APPLE_CC__) || defined(BSD)
                return send(sock, s, len, 0);
            #elif defined(__linux__)
                return send(sock, s, len, MSG_NOSIGNAL);
            #endif
        }

    public:

        Stream() : sock(INVALID_SOCKET), m_timeout(10), m_quality(70)
        {
            FD_ZERO( &master );
        }

        ~Stream() 
        {
            release();
        }

        void configureStream(StringMap & settings);
        bool release();
        bool open();
        bool isOpened();
        bool connect();
        void write(Image image);
        double wait;
    };
}
#endif