#include "capture/Stream.h"

namespace kerberos
{

    // ----------------------------------
    // Configure stream thread settings

    void Stream::configureStream(StringMap & settings)
    {
        //read port from settings
        int port = std::atoi(settings.at("streams.Mjpg.streamPort").c_str());
        int quality = std::atoi(settings.at("streams.Mjpg.quality").c_str());
       
        //use port up to well known ports range
       if(port >= 1024)
       {
           //TODO: here it would be nice to check if port is valid and free
           m_streamPort = port;
           m_quality = quality;
           LINFO << "Configured stream on port " << helper::to_string(m_streamPort) << " with quality: " << helper::to_string(m_quality) ;
       }
       else
       {
           LERROR << "Settings: can't use invalid port";
           //TODO: manage invalid port error
       }
    }

    bool Stream::release()
    {
        for(int i = 0; i < clients.size(); i++)
        {
            shutdown(clients[i], 2);
            FD_CLR(clients[i],&master); 
        }
        
        clients.clear();
        
        if (sock != INVALID_SOCKET)
        {
            shutdown(sock, 2);
            close(sock);
        }
        sock = (INVALID_SOCKET);
        
        return false;
    }

    bool Stream::open()
    {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        
        int reuse = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));

        SOCKADDR_IN address;       
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_family = AF_INET;
        address.sin_port = htons(m_streamPort);
        
        while(bind(sock, (SOCKADDR*) &address, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        {
            LERROR << "Stream: couldn't bind sock";
            release();
            usleep(1000*10000);
            sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        }
        
        while(listen(sock, 2) == SOCKET_ERROR)
        {
            LERROR << "Stream: couldn't listen on sock";
            usleep(1000*10000);
        }
        
        FD_SET(sock, &master);    
        
        return true;
    }

    bool Stream::isOpened() 
    {
        return sock != INVALID_SOCKET; 
    }

    bool Stream::connect()
    {
        fd_set rread = master;
        struct timeval to = {0,m_timeout};
        SOCKET maxfd = sock+1;
        
        if(select( maxfd, &rread, NULL, NULL, &to ) <= 0)
            return true;

        int addrlen = sizeof(SOCKADDR);
        SOCKADDR_IN address = {0};     
        SOCKET client = accept(sock, (SOCKADDR*)&address, (socklen_t*) &addrlen);

        if (client == SOCKET_ERROR)
        {
            LERROR << "Stream: couldn't accept connection on sock";
            LINFO << "Stream: reopening master sock";
            release();
            open();
            return false;
        }
  
        maxfd=(maxfd>client?maxfd:client);
        FD_SET( client, &master );
        _write( client,"HTTP/1.0 200 OK\r\n"
            "Server: Mozarella/2.2\r\n"
            "Accept-Range: bytes\r\n"
            "Max-Age: 0\r\n"
            "Expires: 0\r\n"
            "Cache-Control: no-cache, private\r\n"
            "Pragma: no-cache\r\n"
            "Content-Type: multipart/x-mixed-replace; boundary=mjpegstream\r\n"
            "\r\n",0);
        
        clients.push_back(client);
        packetsSend[client] = 0;

        return true;
    }
    
    void Stream::write(Image image)
    {
        try
        {
            // Check if some clients connected
            // if not drop this shit..
            if(clients.size()==0) return;
            
            // Encode the image
            cv::Mat frame = image.getImage();
            if(frame.cols > 0 && frame.rows > 0)
            {
                std::vector<uchar>outbuf;
                std::vector<int> params;
                params.push_back(cv::IMWRITE_JPEG_QUALITY);
                params.push_back(m_quality);
                cv::imencode(".jpg", frame, outbuf, params);
                int outlen = outbuf.size();

                for(int i = 0; i < clients.size(); i++)
                {
                    packetsSend[clients[i]]++;

                    int error = 0;
                    socklen_t len = sizeof (error);
                    int retval = getsockopt(clients[i], SOL_SOCKET, SO_ERROR, &error, &len);

                    if (retval == 0 && error == 0)
                    {
                        char head[400];
                        sprintf(head,"--mjpegstream\r\nContent-Type: image/jpeg\r\nContent-Length: %lu\r\n\r\n",outlen);

                        _write(clients[i],head,0);

                        retval = getsockopt(clients[i], SOL_SOCKET, SO_ERROR, &error, &len);

                        if (retval == 0 && error == 0)
                        {
                            _write(clients[i],(char*)(&outbuf[0]),outlen);
                        }
                    }

                    if (retval != 0 || error != 0)
                    {
                        shutdown(clients[i], 2);
                        FD_CLR(clients[i],&master);
                        std::vector<int>::iterator position = std::find(clients.begin(), clients.end(), clients[i]);
                        if (position != clients.end())
                        {
                            clients.erase(position);
                        }
                    }
                }
            }
        }
        catch(cv::Exception & ex){}
    }
}                                                       
