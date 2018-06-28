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

    // ----------------------------------
    // Convert information to AWS headers
    // Important: headers need to be sorted, as AWS expect this.
    // otherwise the request is invalid.

    std::vector<std::string> S3::convertMediaInfoToAWSResourceHeaders(std::string path, std::string fileFormat)
    {
        std::vector<std::string> tokens;
        helper::tokenize(path, tokens, "/");
        std::string fileName = tokens[tokens.size()-1];

        std::vector<std::string> extensions;
        helper::tokenize(fileName, extensions, ".");

        // ------------------------------------------------
        // Machine and video/image specific metadata

        std::vector<std::string> canonicalizedAmzHeaders;
        canonicalizedAmzHeaders.push_back("x-amz-meta-capture:" + m_parameters.at("capture"));
        canonicalizedAmzHeaders.push_back("x-amz-meta-productid:" + m_productKey);
        canonicalizedAmzHeaders.push_back("x-amz-meta-publickey:" + m_publicKey);
        canonicalizedAmzHeaders.push_back("x-amz-meta-uploadtime:" + getDate());

        // -----------------------------
        // Set storage type

        canonicalizedAmzHeaders.push_back("x-amz-storage-class:ONEZONE_IA");

        // ------------------------------------------------
        // Get event info from filename (using fileFormat) and convert it
        // to x-amz-meta headers which are stored in the S3 object.
        // timestamp_microseconds_instanceName_regionCoordinates_numberOfChanges_token.jpg
        // The idea is that we not longer store data into the file name, but
        // store them in the metadata key-value store of the S3 object.

        std::vector<std::string> formatNameTokens;
        helper::tokenize(fileFormat, formatNameTokens, ".");

        std::vector<std::string> fileFormatTokens;
        helper::tokenize(formatNameTokens[0], fileFormatTokens, "_");
        std::vector<std::string> fileFormatTokensSorted = fileFormatTokens;
        std::sort(fileFormatTokensSorted.begin(), fileFormatTokensSorted.end());

        std::vector<std::string> mediaValues;
        helper::tokenize(extensions[0], mediaValues, "_");

        for(int i = 0; i < fileFormatTokensSorted.size(); i++)
        {
            std::string element = fileFormatTokensSorted[i];

            std::vector<std::string>::iterator it = std::find(fileFormatTokens.begin(), fileFormatTokens.end(), element);
            int position = distance(fileFormatTokens.begin(), it);

            for (int i=0; element[i]; i++) element[i] = tolower(element[i]);

            canonicalizedAmzHeaders.push_back("x-amz-meta-event-" + element + ":" + mediaValues[position]);
        }

        // ----------------------------------------------
        // Before send the headers, we need to sort them!

        LINFO << "S3: Sending new file to cloud.";

        std::sort(canonicalizedAmzHeaders.begin(), canonicalizedAmzHeaders.end());

        for(int i = 0; i < canonicalizedAmzHeaders.size(); i++)
        {
            BINFO << "S3 - File info: " << canonicalizedAmzHeaders[i];
        }

        return canonicalizedAmzHeaders;
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

        std::string contentType = "image/jpeg";

        std::vector<std::string> extensions;
        helper::tokenize(fileName, extensions, ".");
        std::string extension = helper::urlencode(extensions[extensions.size()-1]);
        std::string fileFormat = "";
        if(extension == "mp4")
        {
            contentType = "video/mp4";
            fileFormat = m_parameters.at("ios.Video.fileFormat");
        }
        else
        {
            fileFormat = m_parameters.at("ios.Disk.fileFormat");
        }

        headers.push_back("Content-Type: " + contentType);

        // Constructing amzHeader object
        std::string canonicalizedAmzHeadersString = "";
        std::vector<std::string> canonicalizedAmzHeaders = convertMediaInfoToAWSResourceHeaders(pathToImage, fileFormat);
        for(int i = 0; i < canonicalizedAmzHeaders.size(); i++)
        {
            std::string amzHeader = canonicalizedAmzHeaders[i];
            headers.push_back(amzHeader);
            canonicalizedAmzHeadersString += amzHeader + "\n";
        }

        // AWS S3 Folder
        std::string folder = (m_folder == "") ? "" : m_folder + "/";

        // Authorize request
        std::string request = "PUT\n";
        request += "\n";
        request += contentType + "\n";
        request += date + "\n";
        request += canonicalizedAmzHeadersString;
        request += "/" + m_bucket + "/" + folder + fileName;
        headers.push_back("Authorization: " + authorize(request));

        // Url
        std::string url = "https://" + m_bucket + ".s3.amazonaws.com/" + folder + fileName;

        // Body
        std::vector<std::string> body;
        body.push_back(pathToImage);

        // Execute HEAD operation
        bool uploaded = put(pathToImage, url, headers, body);

        return uploaded;
    }


    bool S3::put(const std::string & pathToImage, const std::string & url, const std::vector<std::string> & headers, const std::vector<std::string> & body)
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
            curl_easy_setopt(curlHandle, CURLOPT_PROXY, "http://proxy.kerberos.io:80");
            curl_easy_setopt(curlHandle, CURLOPT_TRANSFER_ENCODING, 1L);
            curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curlHandle, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(curlHandle, CURLOPT_READDATA, fd);
            curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curlHandle, CURLOPT_READFUNCTION, reader);
            curl_easy_setopt(curlHandle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &output);

            BINFO << "S3: uploading image to bucket.";

            result = curl_easy_perform(curlHandle); //Perform
            long http_code = 0;
            curl_easy_getinfo (curlHandle, CURLINFO_RESPONSE_CODE, &http_code);
            curl_easy_cleanup(curlHandle);
            curl_slist_free_all(httpHeaders);
            fclose(fd);

            if(http_code == 200 && result != CURLE_ABORTED_BY_CALLBACK) {

              // Check if file really was uploaded.
              // We'll query the S3 bucket and check if it's there.

              LINFO << "S3: file uploaded.";

              return doesExist(pathToImage);

            }
            else if (http_code == 403) {

              // User is not allowed to push with these credentials.
              // We remove the symbol.

              LINFO << "S3: permission denied, your file wasn't uploaded.";

              return true;

            }

            else {

              LINFO << "S3: file was not uploaded, something went wrong. Please check if you internet connectivity works.";

            }
        }

        return false;
    }


    bool S3::doesExist(std::string pathToImage)
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
        std::string request = "HEAD\n";
        request += "\n";
        request += "\n";
        request += date + "\n";
        request += "/" + m_bucket + "/" + folder + fileName;

        headers.push_back("Authorization: " + authorize(request));

        // Url
        std::string url = "https://" + m_bucket + ".s3.amazonaws.com/" + folder + fileName;

        // Execute get operation
        return head(url, headers);
    }

    bool S3::head(const std::string & url, const std::vector<std::string> & headers)
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
            curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "HEAD");
            curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &output);

            BINFO << "S3: checking if file exists in bucket.";

            result = curl_easy_perform(curlHandle); //Perform
            long http_code = 0;
            curl_easy_getinfo (curlHandle, CURLINFO_RESPONSE_CODE, &http_code);
            curl_easy_cleanup(curlHandle);
            curl_slist_free_all(httpHeaders);

            if(http_code == 200 && result != CURLE_ABORTED_BY_CALLBACK)
            {
                LINFO << "S3: file exists in bucket, succesfully uploaded.";
                return true;
            }

            LINFO << "S3: file wasn't uploaded, something went wrong.";

            return false;
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
