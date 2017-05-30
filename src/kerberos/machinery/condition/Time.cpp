#include "machinery/condition/Time.h"

namespace kerberos
{
    void Time::setup(const StringMap & settings)
    {
        Condition::setup(settings);
        std::vector<std::pair<std::string, std::string> > times;
    
        std::vector<std::string> days;
        helper::tokenize(settings.at("conditions.Time.times").c_str(), days, "-");
        
        for(int i = 0; i < days.size(); i++)
        {
            std::vector<std::string> startAndStop;
            helper::tokenize(days[i], startAndStop, ",");
            times.push_back(std::make_pair(startAndStop[0], startAndStop[1]));
        }

        setTimes(times);
        std::string timezone = settings.at("timezone");
        std::replace(timezone.begin(), timezone.end(), '-', '/');
        std::replace(timezone.begin(), timezone.end(), '$', '_');
        setTimezone(timezone);
        
        setDelay(std::atoi(settings.at("conditions.Time.delay").c_str()));
    }

    bool Time::allowed(const ImageVector & images)
    {
        std::string currentDateTime = helper::currentDateTime(m_timezone);
        
        // Split currentDateTime by space
        std::vector<std::string> tokens;
        helper::tokenize(currentDateTime, tokens, " ");

        int dayOfWeek = std::atoi(tokens[2].c_str())-1;
        
        // Get time and remove leading zero (e.g. 09:10:23 -> 9:10:23)
        std::string currentTime = tokens[1];
        if(currentTime[0] == '0') currentTime.erase(0, 1);
        
        std::string from = m_times[dayOfWeek].first + ":00";
        std::string to = m_times[dayOfWeek].second + ":00";
        
        int isAllowedFrom = helper::compareTime(currentTime, from);
        int isAllowedTo = helper::compareTime(to, currentTime);
        bool isAllowed = (isAllowedFrom >= 0 && isAllowedTo >= 0);

        if(!isAllowed)
        {
            BINFO << "Condition: not in time interval.";
            usleep(getDelay()*1000);
        }
        
        return isAllowed;
    }
}
