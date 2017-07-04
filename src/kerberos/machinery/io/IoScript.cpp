#include "machinery/io/IoScript.h"

namespace kerberos
{
    void IoScript::setup(const StringMap & settings)
    {
        Io::setup(settings);
        
        // --------------------------
        // Get name from instance
        
        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);
        
        // ------------------
        // Get path to script
        
        setPath(settings.at("ios.Script.path").c_str());
    }
    
    bool IoScript::save(Image & image, JSON & data)
    {
        // ---------------------------------------
        // Attach additional fields to JSON object
        
        JSON dataCopy;
        JSON::AllocatorType& allocator = dataCopy.GetAllocator();
        dataCopy.CopyFrom(data, allocator);
        
        JSONValue instanceName;
        instanceName.SetString(getInstanceName().c_str(), allocator);
        dataCopy.AddMember("instanceName", instanceName, allocator);
        
        // -----------------------------
        // Convert JSON object to string
        
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        dataCopy.Accept(writer);
        
        // -------------------
        // Send a post to URL

        std::string path = (std::string) getPath();
        BINFO << "IoScript: calling script at " + path;

        if(path != "")
        {
            std::string command = "bash " + path + " '" +  buffer.GetString() + "'";
            system(command.c_str());
            return true;
        }
        
        return false;
    }
}
