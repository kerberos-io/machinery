#include "Helper.h"

namespace kerberos
{
    namespace helper
    {
        void getFilesInDirectory(std::vector<std::string> &out, const std::string &directory)
        {
            DIR *dir;
            struct dirent *ent;
            struct stat st;

            dir = opendir(directory.c_str());
            if(dir)
            {
                while ((ent = readdir(dir)) != NULL)
                {
                    const std::string file_name = ent->d_name;
                    const std::string full_file_name = directory + "/" + file_name;
                    
                    if (file_name[0] == '.')
                        continue;
                    
                    if (stat(full_file_name.c_str(), &st) == -1)
                    continue;
                    const bool is_directory = (st.st_mode & S_IFDIR) != 0;

                    if (is_directory)
                        continue;

                    out.push_back(full_file_name);
                }
                closedir(dir);
            }
        }

        std::string urlencode(const std::string &value)
        {
            std::ostringstream escaped;
            escaped.fill('0');
            escaped << std::hex;

            for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
                std::string::value_type c = (*i);

                // Keep alphanumeric and other accepted characters intact
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                    escaped << c;
                    continue;
                }

                // Any other characters are percent-encoded
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << int((unsigned char) c);
                escaped << std::nouppercase;
            }

            return escaped.str();
        }
        
        const char * getValueByKey(kerberos::StringMap & map, const std::string & key)
        {
            if(map.find(key) != map.end())
            {
                return map.at(key).c_str();
            }
        
            return 0;
        }
    
        kerberos::StringMap getCommandOptions(int argc, char ** argv)
        {
            kerberos::StringMap parameters;
            
            char ** begin = argv;
            char ** end = argv + argc;
            
            while(begin != end)
            {
                std::string option = *begin;
                if(option.substr(0,2) == "--")
                {
                    option = option.substr(2,option.size());
                    std::string value = *(++begin);
                    if(value.substr(0,2) != "--")
                    {
                        parameters[option] = value;
                    }
                    else
                    {
                        --begin;
                    }
                }
                begin++;
            }
            
            return parameters;
        }

        void getSettingsFromXML(TiXmlElement * root, std::string prefix, kerberos::StringMap & settings);
        kerberos::StringMap getSettingsFromXML(const std::string & path)
        {
            kerberos::StringMap settings;
            
            std::string directory = path.substr(0,path.rfind('/')) + "/" ;
        
            TiXmlDocument doc(path.c_str());
            if(doc.LoadFile())
            {
                TiXmlNode * root = doc.FirstChildElement("kerberos");
                if(root)
                {
                    root = root->FirstChildElement("instance");
                    TiXmlElement * node = root->FirstChildElement();
                    while (node)
                    {
                        settings[node->Value()] = node->ToElement()->GetText();
                        if(node->Attribute("file") != 0)
                        {
                            // read configuration file..
                            std::string path = directory + node->Attribute("file");
                            TiXmlDocument doc(path.c_str());
                            if(doc.LoadFile())
                            {
                                TiXmlElement * subnode = doc.FirstChildElement();
                                getSettingsFromXML(subnode, subnode->Value(), settings);
                            }
                        }
                        node = node->NextSiblingElement();
                    }
                }
            }
            else
            {
                throw TinyXMLOpenFileException();
            }
            return settings;
        }
        
        void getSettingsFromXML(TiXmlElement * root, std::string prefix, kerberos::StringMap & settings)
        {
            // First element is wrapper
            TiXmlElement * node = root->FirstChildElement();
            while (node)
            {
                if(node->FirstChild())
                {
                    if(node->FirstChild()->Type()==TiXmlNode::TINYXML_TEXT)
                    {
                        settings[prefix + "." + node->Value()] = node->ToElement()->GetText();
                    }
                    else
                    {
                        getSettingsFromXML(node, prefix + "." + node->Value(), settings);
                    }
                }
                else
                {
                    settings[prefix + "." + node->Value()] = "";
                }
                node = node->NextSiblingElement();
            }
        }
        
        std::string printStringMap(const std::string & prefix, const kerberos::StringMap & map)
        {
            std::string output = prefix + "\n";
            
            for(kerberos::StringMap::const_iterator it = map.begin(); it != map.end(); it++)
            {
                output += "- " + it->first + " = " + it->second + "\n";
            }
            
            output.erase( output.end()-1, output.end());
            
            return output;
        }
        
        std::string to_string(const int & t)
        {
            std::stringstream ss;
            ss << t;
            return ss.str();
        }
        
        bool replace(std::string& str, const std::string& from, const std::string& to)
        {
            size_t start_pos = str.find(from);
            if(start_pos == std::string::npos)
                return false;
            str.replace(start_pos, from.length(), to);
            return true;
        }
        
        std::string generatePath(const std::string timezone, const std::string & subDirectory)
        {
            srand(int(time(NULL)));
            std::string pathToImage;
            time_t t = time(0);
            
            pathToImage = ((subDirectory!="")?subDirectory+"/":"") + to_string(t) + "_" + to_string(rand()%10000) + ".jpg";
            return pathToImage;
        }

        std::string getTimestamp()
        {
            srand(int(time(NULL)));
            std::string timestamp;
            time_t t = time(0);
            
            timestamp = to_string(t);
            
            return timestamp;
        }

        std::string getMicroseconds()
        {
            std::string microseconds;
            struct timeval microTime; 
            gettimeofday(&microTime, NULL);

            microseconds = to_string(microTime.tv_usec);
            
            return microseconds;
        }
        
        // -------------------------------------------------
        // Get current date tim: year-month-day time format
        
        const std::string currentDateTime(std::string timezone)
        {
            struct tm tstruct;
            char buf[80];
            
            time_t now = time(0);

            char * timeformat = "%d-%m-%Y %X %u";
            if(timezone != "")
            {
                setenv("TZ", timezone.c_str(), 1);
                tzset();
            }
            
            tstruct = *localtime(&now);
            strftime(buf, sizeof(buf), timeformat, &tstruct);
            return buf;
        }
        
        // ---------------------------------------------
        // Compare two time strings from format h:mm:ss
        
        int compareTime(const std::string & first, const std::string & second)
        {
            std::vector<std::string> firstTimeTokens, secondTimeTokens;
            tokenize(first, firstTimeTokens, ":");
            tokenize(second, secondTimeTokens, ":");
            
            enum {smaller = -1, equal = 0, bigger = 1};
            
            for(int i = 0; i < std::min(firstTimeTokens.size(),secondTimeTokens.size()); i++)
            {
                if(std::atoi(firstTimeTokens[i].c_str()) < std::atoi(secondTimeTokens[i].c_str()))
                {
                    return smaller;
                }
                else if(std::atoi(firstTimeTokens[i].c_str()) > std::atoi(secondTimeTokens[i].c_str()))
                {
                    return bigger;
                }
            }
            
            return equal;
        }
        
        void tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters)
        {
            // Skip delimiters at beginning.
            std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
            
            // Find first non-delimiter.
            std::string::size_type pos = str.find_first_of(delimiters, lastPos);
            
            while (std::string::npos != pos || std::string::npos != lastPos)
            {
                // Found a token, add it to the vector.
                tokens.push_back(str.substr(lastPos, pos - lastPos));
                
                // Skip delimiters.
                lastPos = str.find_first_not_of(delimiters, pos);
                
                // Find next non-delimiter.
                pos = str.find_first_of(delimiters, lastPos);
            }
        }
        
        // -----------------------------------------------------------------
        // Calculate working directory
        //   - uses getcwd(), argv[0] and path to binary (relative to cwd)
        //   - to determine the working directory.
        
        const std::string getRootDirectory(const char * relativeDirectory)
        {
            char buffer[100];
            char * cwd = getcwd(buffer, sizeof(buffer));
            if(cwd != 0)
            {
                return normalizePath(std::string(cwd) + "/", std::string(relativeDirectory) + "/", "/bin/kerberos");
            }
            return "";
        }
        
        std::string normalizePath(const std::string & cwd, const std::string & command, const std::string & binaryPath)
        {
            std::string path;
            std::string pattern;
            
            std::string::const_iterator start;
            std::string::const_iterator stop;
            
            if(command.size() > 0 && command[0] == '/')
            {
                path = command;
            }
            else
            {
                path = cwd + command;
                
                start = path.begin();
                stop = path.end();
                
                pattern = "../";
                int occurence = path.find(pattern);
                while(occurence > -1)
                {
                    start += occurence;
                    stop = start + pattern.size();
                    
                    int position = start - path.begin() - 2;
                    if(position > 0)
                    {
                        position = path.rfind('/', position);
                        position = (position < 1)?0:position;
                        start = path.begin() + position;
                        start = (*(start+1) == '/')? start + 1: start;
                        stop -= 1;
                    }
                    
                    path.erase(start - path.begin(), stop - start);
                    occurence = path.find(pattern);
                    start = path.begin();
                    stop = path.end();
                }
                
                pattern = "./";
                occurence = path.find(pattern);
                while(occurence > -1)
                {
                    start = path.begin() + occurence;
                    stop = start + pattern.size();
                    
                    path.erase(start - path.begin(), stop - start);
                    occurence = path.find(pattern);
                }
            }
            
            pattern = binaryPath;
            int occurence = path.find(pattern);
            if(occurence > -1)
            {
                start = path.begin() + occurence;
                stop = start + pattern.size();
                path.erase(start - path.begin(), stop - start);
            }
            
            path = ((path.begin() != path.end() && *path.begin() != '/')?"/":"") + path;
            
            return path;
        }
    }
}
