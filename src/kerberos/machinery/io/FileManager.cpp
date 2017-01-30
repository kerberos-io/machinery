#include "machinery/io/FileManager.h"
#if defined(_WIN32)
	#include <direct.h>
#endif

namespace kerberos
{
    // -------------------------
    //	Check if the directory exists, if not create the directory.
    //	This function will create a new directory if the image is the first
    //	image taken for a specific day
    bool FileManager::createDirectoryIfNotExists(std::string & dir)
    {
        struct stat info;
        std::string path = m_baseDir + "/" + dir;
	   // -------------------------
        // If directory doesn't exists, can't open.
        if(stat(path.c_str(), &info ) != 0)
        {
            return createDirectory(path);
        }
        return false;
    }

    bool FileManager::createDirectoryIfNotExists(char * dir)
    {
        std::string d = std::string(dir);
        return createDirectoryIfNotExists(d);
    }

    // -------------------------
    //	Create a directory for a give path
    //		- Parent directory has to exist, no recursive creation!
    bool FileManager::createDirectory(const char * path)
    {
        #if defined(_WIN32)
            return _mkdir(path);
        #else
            return mkdir(path, 0777);
        #endif
    }

    bool FileManager::createDirectory(std::string & path)
    {
        return createDirectory(path.c_str());
    }

    bool FileManager::save(Image & image, const std::string & path, bool createSymbol)
    {
        // -------------------------
        // Create filename and directories if not exist
        
        unsigned long found = path.find("/");
        while(found != -1)
        {
            std::string subDirectory = path.substr(0, found);
            createDirectoryIfNotExists(subDirectory);
            found = path.find("/",found+1);
        }
        // -------------------------
        // Call save method on image
        
        std::string pathToImage = getBaseDirectory() + "/" + path;
        if(image.save(pathToImage))
        {
            if(createSymbol)
            {
                std::string link = SYMBOL_DIRECTORY + path;
                symlink(pathToImage.c_str(), link.c_str());
            }

            return true;
        }
        else
        {
            return false;
        }
            
    }
}