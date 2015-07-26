#include "machinery/io/IoDisk.h"

namespace kerberos
{
    void IoDisk::setup(const StringMap & settings)
    {
        Io::setup(settings);
        
        // --------------------------
        // Get name from instance
        
        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);
        
        // -------------------------------------------------------------
        // Filemanager is mapped to a directory and is used by an image
        // to save to the correct directory.
        
        setFileFormat(settings.at("ios.Disk.fileFormat"));
        m_fileManager.setBaseDirectory(settings.at("ios.Disk.directory"));
    }

    std::string IoDisk::buildPath(std::string pathToImage)
    {
        // -----------------------------------------------
        // Get timestamp, microseconds, random token, and instance name
        
        std::string instanceName = getInstanceName();
        kerberos::helper::replace(pathToImage, "instanceName", instanceName);
        
        std::string timestamp = kerberos::helper::getTimestamp();
        kerberos::helper::replace(pathToImage, "timestamp", timestamp);

        std::string microseconds = kerberos::helper::getMicroseconds();
        std::string size = kerberos::helper::to_string((int)microseconds.length());
        kerberos::helper::replace(pathToImage, "microseconds", size + "-" + microseconds);
        
        std::string token = kerberos::helper::to_string(rand()%1000);
        kerberos::helper::replace(pathToImage, "token", token);

        return pathToImage;
    }
    
    bool IoDisk::save(Image & image)
    {
        // ----------------------------------------
        // The naming convention that will be used
        // for the image.
        
        std::string pathToImage = getFileFormat();
        
        // ---------------------
        // Replace variables

        pathToImage = buildPath(pathToImage);
        
        // ---------------------------------------------------------------------
        // Save original version & generate unique timestamp for current image
        
        return m_fileManager.save(image, pathToImage);
    }
    
    bool IoDisk::save(Image & image, JSON & data)
    {
        // ----------------------------------------
        // The naming convention that will be used
        // for the image.
        
        std::string pathToImage = getFileFormat();

        // ------------------------------------------
        // Stringify data object: build image path
        // with data information.
        
        static const std::string kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
        for (JSONValue::ConstMemberIterator itr = data.MemberBegin(); itr != data.MemberEnd(); ++itr)
        {
            std::string name = itr->name.GetString();
            std::string type = kTypeNames[itr->value.GetType()];
            
            if(type == "String")
            {
                std::string value = itr->value.GetString();
                kerberos::helper::replace(pathToImage, name, value);
            }
            else if(type == "Number")
            {
                std::string value = kerberos::helper::to_string(itr->value.GetInt());
                kerberos::helper::replace(pathToImage, name, value);
            }
            else if(type == "Array")
            {
                std::string arrayString = "";
                for (JSONValue::ConstValueIterator itr2 = itr->value.Begin(); itr2 != itr->value.End(); ++itr2)
                {
                    type = kTypeNames[itr2->GetType()];
                    
                    if(type == "String")
                    {
                        arrayString += itr2->GetString();
                    }
                    else if(type == "Number")
                    {
                       arrayString += kerberos::helper::to_string(itr2->GetInt());
                    }
                    
                    arrayString += "-";
                }
                kerberos::helper::replace(pathToImage, name, arrayString.substr(0, arrayString.size()-1));
            }
        }
        
        /// ---------------------
        // Replace variables

        pathToImage = buildPath(pathToImage);
        
        // -------------------------------------------------------
        // Add path to JSON object, so other IO devices can use it
        
        JSONValue path;
        JSON::AllocatorType& allocator = data.GetAllocator();
        path.SetString(pathToImage.c_str(), allocator);
        data.AddMember("pathToImage", path, allocator);
        
        // ---------------------------------------------------------------------
        // Save original version & generate unique timestamp for current image

        return m_fileManager.save(image, pathToImage);
    }
}
