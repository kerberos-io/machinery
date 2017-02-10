//
//  Class: GoogleDrive
//  Description: Google drive storage
//  Created:     08/02/2015
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

#ifndef __GoogleDrive_H_INCLUDED__   // if GoogleDrive.h hasn't been included yet...
#define __GoogleDrive_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "cloud/Cloud.h"
#include "Executor.h"
#include <curl/curl.h>
#include <time.h>
#include "HMAC_SHA1.h"
#include "base64.h"

namespace kerberos
{
    char GoogleDriveName[] = "GoogleDrive";
    class GoogleDrive : public CloudCreator<GoogleDriveName, GoogleDrive>
    {
        private:
        
            std::string m_bucket;
            std::string m_folder;
            std::string m_publicKey;
            std::string m_privateKey;
            
        public:
        
            GoogleDrive(){};
            virtual ~GoogleDrive(){};
        
            void setup(StringMap & settings);
            void setBucket(std::string bucket);
            void setFolder(std::string folder);
            void setPublicKey(std::string key);
            void setPrivateKey(std::string key);
        
            bool upload(std::string pathToImage);
    };
}

#endif