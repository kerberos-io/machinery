#include "cloud/Cloud.h"

namespace kerberos
{
    void Cloud::setup(kerberos::StringMap & settings)
    {
        // -------------------------------
        // Upload interval [0.5sec;4.2min]
        
        m_min = 1500;
        m_max = 256000;
        m_interval = m_min;
    }
    
    void Cloud::scan()
    {
        for(;;)
        {
            usleep(m_min*1000);
            
            std::vector<std::string> storage;
            helper::getFilesInDirectory(storage, SYMBOL_DIRECTORY); // get all symbol links of directory
            
            std::vector<std::string>::iterator it = storage.begin();

            while(it != storage.end())
            { 
                std::string file = *it;
                bool hasBeenUploaded = upload(file);
                
                if(hasBeenUploaded)
                {
                    unlink(file.c_str()); // remove symbol link
                    m_interval = m_min; // reset interval
                }
                else
                {   
                    m_interval = (m_interval * 2 < m_max) ? m_interval * 2 : m_max; // update interval exponential.
                    break;
                }
                
                it++;
            }
        }
    }
}