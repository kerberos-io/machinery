#include "cloud/GoogleDrive.h"

namespace kerberos
{
    void GoogleDrive::setup(kerberos::StringMap & settings)
    {
        Cloud::setup(settings);
        
        // -------------------------
        // Initialize GoogleDrive credentials
        
        setBucket(settings.at("clouds.GoogleDrive.bucket"));
        setFolder(settings.at("clouds.GoogleDrive.folder"));
        setPublicKey(settings.at("clouds.GoogleDrive.publicKey"));
        setPrivateKey(settings.at("clouds.GoogleDrive.privateKey"));
    }
    
    void GoogleDrive::setBucket(std::string bucket)
    {
        m_bucket = bucket;
    }
    
    void GoogleDrive::setFolder(std::string folder)
    {
        m_folder = folder;
    }
    
    void GoogleDrive::setPublicKey(std::string key)
    {
        m_publicKey = key;
    }
    
    void GoogleDrive::setPrivateKey(std::string key)
    {
        m_privateKey = key;
    }
    
    bool GoogleDrive::upload(std::string pathToImage)
    {
        return true;
    }
}