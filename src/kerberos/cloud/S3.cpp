#include "cloud/S3.h"

namespace kerberos
{
    void S3::setup(kerberos::StringMap & settings)
    {
        Cloud::setup(settings);
        
        // -------------------------
        // Initialize S3 credentials
        
        setBucket(settings.at("clouds.S3.bucket"));
        setFolder(settings.at("clouds.S3.folder"));
        setPublicKey(settings.at("clouds.S3.publicKey"));
        setPrivateKey(settings.at("clouds.S3.privateKey"));
    }
    
    void S3::setBucket(std::string bucket)
    {
        m_bucket = bucket;
    }
    
    void S3::setFolder(std::string folder)
    {
        m_folder = folder;
    }
    
    void S3::setPublicKey(std::string key)
    {
        m_publicKey = key;
    }
    
    void S3::setPrivateKey(std::string key)
    {
        m_privateKey = key;
    }
    
    std::string S3::authorize(const std::string request)
    {
        BYTE digest[20];
        CHMAC_SHA1 hmac_sha1;
        hmac_sha1.HMAC_SHA1((BYTE*)request.c_str(), request.size(),(BYTE*)m_privateKey.c_str(),m_privateKey.size(),digest);
        std::string signature(base64_encode((unsigned char*)digest,20));
        return "AWS " + m_publicKey + ":" + signature;
    }
    
    std::string S3::getDate()
    {
        time_t lt;
        time(&lt);
        struct tm * tmTmp;
        tmTmp = gmtime(&lt);

        char buf[37];
        strftime(buf,37,"%a, %d %b  %Y %X GMT",tmTmp);

        std::string stringBuffer(buf);
        return stringBuffer; 
    }
    
    bool S3::upload(std::string pathToImage)
    {
        std::vector<std::string> headers;
    
        // File name
        std::vector<std::string> tokens;
        helper::tokenize(pathToImage, tokens, "/");
        std::string fileName = helper::urlencode(tokens[tokens.size()-1]);

        // Host URL
        headers.push_back("Host: " + m_bucket + ".s3.amazonaws.com");

        // Date
        std::string date = getDate();
        headers.push_back("Date: " + date);

        // folder    
        std::string folder = (m_folder == "") ? "" : m_folder + "/";
        
        // Authorize request
        std::string request = "PUT\n";
        request += "\n";
        request += "\n";
        request += date + "\n";
        request += "/" + m_bucket + "/" + folder + fileName;
        headers.push_back("Authorization: " + authorize(request));

        // Url
        std::string url = "https://" + m_bucket + ".s3.amazonaws.com/" + folder + fileName;

        // Body
        std::vector<std::string> body;
        body.push_back(pathToImage);

        // Execute get operation
        return put(url, headers, body);
    }
    

    bool S3::put(const std::string & url, const std::vector<std::string> & headers, const std::vector<std::string> & body)
    {
        // If no credentials are set do nothing..
        if(m_bucket == "" || m_folder == "" || m_privateKey == "" || m_publicKey == "") return false;
        
        // Initialize curl 
        curl_global_init(CURL_GLOBAL_ALL);
        CURL* curlHandle = curl_easy_init();
        CURLcode result;

        // Initialize headers
        curl_slist* httpHeaders = NULL;
        for(int i = 0; i < headers.size(); i++)
        {
            httpHeaders = curl_slist_append(httpHeaders, headers[i].c_str());
        }

        // Intialize body
        std::string file = body[0];
        FILE * fd = fopen(file.c_str(), "rb"); /* open file to upload */  
        struct stat file_info;
        fstat(fileno(fd), &file_info);
        
        // Execute
        std::string output = "";
        if(curlHandle) 
        {
            curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION,1);//Set automaticallly redirection
            curl_easy_setopt(curlHandle, CURLOPT_MAXREDIRS,1);//Set max redirection times
            curl_easy_setopt(curlHandle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);//Set http version 1.1fer
            curl_easy_setopt(curlHandle, CURLOPT_HEADER, true);//Set header true
            curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, httpHeaders);//Set headers
            curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());//Set URL
            curl_easy_setopt(curlHandle, CURLOPT_TRANSFER_ENCODING, 1L); 
            curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curlHandle, CURLOPT_UPLOAD, 1L); 
            curl_easy_setopt(curlHandle, CURLOPT_READDATA, fd);
            curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curlHandle, CURLOPT_READFUNCTION, reader);
            curl_easy_setopt(curlHandle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &output);

            BINFO << "S3: uploading image to bucket.";

            result = curl_easy_perform(curlHandle);//Perform
            curl_easy_cleanup(curlHandle);
            curl_slist_free_all(httpHeaders);
            fclose(fd);

            return (result == CURLE_OK);
        }

        return false;
    }

    size_t S3::write(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    size_t S3::reader(void *ptr, size_t size, size_t nmemb, FILE *stream)
    {
        size_t retcode = fread(ptr, size, nmemb, stream);
        return retcode;
    }
}