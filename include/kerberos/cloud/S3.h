//
//  Class: S3
//  Description: Simple Storage Service of AWS
//  Created:     05/02/2015
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

#ifndef __S3_H_INCLUDED__   // if S3.h hasn't been included yet...
#define __S3_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "cloud/Cloud.h"
#include "Executor.h"
#include <curl/curl.h>
#include <time.h>
#include "HMAC_SHA1.h"
#include "base64.h"

namespace kerberos
{
    char S3Name[] = "S3";
    class S3 : public CloudCreator<S3Name, S3>
    {
        private:
        
            std::string m_bucket;
            std::string m_folder;
            std::string m_publicKey;
            std::string m_privateKey;
            
        public:
        
            S3(){};
            virtual ~S3(){};
        
            void setup(StringMap & settings);
            void setBucket(std::string bucket);
            void setFolder(std::string folder);
            void setPublicKey(std::string key);
            void setPrivateKey(std::string key);
        
            bool upload(std::string pathToImage);
            std::string authorize(const std::string request);
            std::string getDate();
            bool put(const std::string & url, const std::vector<std::string> & headers, const std::vector<std::string> & body);
            static size_t write(void *contents, size_t size, size_t nmemb, void *userp);
            static size_t reader(void *ptr, size_t size, size_t nmemb, FILE *stream);
    };
}

#endif