#include <fcntl.h>
#include "capture/Stream.h"

namespace kerberos
{

    // ----------------------------------
    // Configure stream thread settings

    void Stream::configureStream(StringMap & settings)
    {
        //read port from settings
        int enabled = (settings.at("streams.Mjpg.enabled") == "true");
        int port = std::atoi(settings.at("streams.Mjpg.streamPort").c_str());
        int quality = std::atoi(settings.at("streams.Mjpg.quality").c_str());
        int fps = std::atoi(settings.at("streams.Mjpg.fps").c_str());
        std::string username = settings.at("streams.Mjpg.username");
        std::string password = settings.at("streams.Mjpg.password");

        //use port up to well known ports range
        if(port >= 1024)
        {
            //TODO: here it would be nice to check if port is valid and free
            m_enabled = enabled;
            m_username = username;
            m_password = password;
            m_streamPort = port;
            m_quality = quality;
            wait = 1. / fps;
        }
        else
        {
            LERROR << "Settings: can't use invalid port";
            //TODO: manage invalid port error
        }
    }

    bool Stream::hasClients()
    {
        return (clients.size() > 0);
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

        LINFO << "Stream: Succesfully closed streaming";

        return false;
    }

    bool Stream::open()
    {
        if(m_enabled)
        {
            sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	          int reuse = 1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));

            struct timeval timeout;
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&timeout,sizeof(timeout));

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

            LINFO << "Stream: Configured stream on port " << helper::to_string(m_streamPort) << " with quality: " << helper::to_string(m_quality);

            return true;
        }

        return false;
    }

    bool Stream::isOpened()
    {
        return sock != INVALID_SOCKET;
    }

    std::map<std::string, std::string> Stream::getRequestInfo(SOCKET client)
    {
        std::map<std::string, std::string> info;

        // We need some information from the client
        // Get data from the request.
        char method[10]={'\0'};
        char url[512]={'\0'};
        char protocol[10]={'\0'};

        unsigned short int length = 1023;
        char buffer[1024] = {'\0'};
        int nread = read(client, buffer, length);
        sscanf (buffer, "%9s %511s %9s", method, url, protocol);

        info["buffer"] = (std::string) buffer;
        info["method"] = (std::string) method;
        info["url"] = (std::string) url;
        info["protocol"] = (std::string) protocol;

        return info;
    }

    bool Stream::authenticate(std::map<std::string, std::string> & requestInfo)
    {
        // Check if we have authentication enabled.
        // verify if both username and password is set.
        if(m_username != "" && m_password != "")
        {
            std::string credentials = m_username +  ":" + m_password;
            std::string credentialsBase64 =  base64_encode((unsigned char const*) credentials.c_str(), credentials.length());

            std::string payload = requestInfo.find("buffer")->second;

            // We'll check if there is an authentication header
            // in the request send by the user.
            int findAuthenticationInHeader = payload.find("Basic", 0);

            if(findAuthenticationInHeader != std::string::npos)
            {
                // We need to find the token in the payload, therefore
                // we'll split the payload by a spaces, and add them to
                // a vector sequentially. After this we'll search for Basic again,
                // and if found we take the next entry in the vector (as this will be the token).

                bool tokenFound = false;
                std::string token = "";

                std::istringstream payloadBySpaces(payload);
                std::string line;

                while (std::getline(payloadBySpaces, line, ' ' ) && !tokenFound)
                {
                    if(line == "Basic")
                    {
                        std::getline(payloadBySpaces, token, ' ');
                        token = token.substr(0 ,token.find('\r'));
                        tokenFound = true;
                    }
                }

                if(!tokenFound)
                {
                    //LERROR << "Stream: no token found in client request.";
                    LERROR << "Stream: no token found in client request.";
                    return false;
                }
                else if(token != credentialsBase64)
                {
                    //LERROR << "Stream: token found, but it's not correct.";
                    LERROR << "Stream: token found, but it's not correct.";
                    return false;
                }

                return true;
            }
            else
            {
                //LERROR << "Stream: no token found in client request.";
                LERROR << "Stream: no token found in client request.";
                return false;
            }

            return false;
        }

        return true;
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
            //LERROR << "Stream: couldn't accept connection on sock";
            //LERROR << "Stream: reopening master sock";
            LERROR << "Stream: couldn't accept connection on sock";
            LERROR << "Stream: reopening master sock";
            release();
            open();
            return false;
        }

        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&timeout, sizeof(timeout));

        maxfd=(maxfd>client?maxfd:client);
        FD_SET( client, &master );

        std::map<std::string, std::string> requestInfo = getRequestInfo(client);

        // If requesting the favicon, don't do anything..
        if(requestInfo["url"] == "/favicon.ico")
        {
            return false;
        }

        bool isAuthenticated = authenticate(requestInfo);

        if(isAuthenticated)
        {
            _write( client,"HTTP/1.0 200 OK\r\n"
                "Server: Mozarella/2.2\r\n"
                "Accept-Range: bytes\r\n"
                "Max-Age: 0\r\n"
                "Expires: 0\r\n"
                "Cache-Control: no-cache, private\r\n"
                "Pragma: no-cache\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Content-Type: multipart/x-mixed-replace; boundary=mjpegstream\r\n"
                "\r\n",0);

            LINFO << "Stream: authentication success";
            LINFO << "Stream: opening socket for new client.";
            clients.push_back(client);
            packetsSend[client] = 0;

            return true;
        }
        else
        {
            const char * unauthorized =
                  "HTTP/1.0 401 Authorization Required\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "WWW-Authenticate: Basic realm=\"Kerberos.io Security Access\"\r\n\r\n\r\n";

            // Request for authorization
            char response[1024]={'\0'};
            snprintf (response, sizeof (response), unauthorized, requestInfo["method"].c_str());
            _write (client, response, strlen(response));

            //LINFO << "Stream: authentication failed.";
            LOG(INFO) << "Stream: authentication failed.";

            FD_CLR(client, &master);
            shutdown(client, 2);
            close(client);

            return false;
        }
    }

    void Stream::writeRAW(uint8_t* data, int32_t length)
    {
      try
      {
          // Check if some clients connected
          // if not drop this shit..
          if(clients.size()==0) return;

          if(length > 0)
          {
              for(int i = 0; i < clients.size(); i++)
              {
                  packetsSend[clients[i]]++;

                  int error = 0;
                  socklen_t len = sizeof (error);
                  int retval = getsockopt(clients[i], SOL_SOCKET, SO_ERROR, &error, &len);
                  int socketState = 0;
                  if (retval == 0 && error == 0)
                  {
                      char head[400];
                      sprintf(head,"--mjpegstream\r\nContent-Type: image/jpeg\r\nContent-Length: %lu\r\n\r\n",length);
                      socketState = _write(clients[i],head,0);
                      retval = getsockopt(clients[i], SOL_SOCKET, SO_ERROR, &error, &len);

                      if (retval == 0 && error == 0)
                      {
                          socketState = _write(clients[i], (char*) data, length);
                      }
                  }

                  if (retval != 0 || error != 0 || socketState == -1)
                  {
                      FD_CLR(clients[i],&master);
                      shutdown(clients[i], 2);
                      close(clients[i]);

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

                    int socketState = 0;
                    char buffer[1024];
                    if (retval == 0 && error == 0)
                    {
                        char head[400];
                        sprintf(head,"--mjpegstream\r\nContent-Type: image/jpeg\r\nContent-Length: %lu\r\n\r\n",outlen);

                        socketState = _write(clients[i],head,0);

                        retval = getsockopt(clients[i], SOL_SOCKET, SO_ERROR, &error, &len);

                        if (retval == 0 && error == 0)
                        {
                            socketState = _write(clients[i],(char*)(&outbuf[0]),outlen);
                        }
                    }

                    if (retval != 0 || error != 0 || socketState == -1)
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
