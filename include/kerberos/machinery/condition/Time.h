//
//  Class: Time
//  Description: When time is in a certain interval return true.
//  Created:     17/07/2014
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

#ifndef __Time_H_INCLUDED__   // if Time.h hasn't been included yet...
#define __Time_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/condition/Condition.h"

namespace kerberos
{
    char TimeName[] = "Time";
    class Time : public ConditionCreator<TimeName, Time>
    {
        private:
            std::vector<std::pair<std::string, std::string> > m_times;
            std::string m_timezone;

        public:
            Time(){}
            void setup(const StringMap & settings);
            void setTimes(std::vector<std::pair<std::string, std::string> > times){m_times = times;};
            void setTimezone(std::string timezone){m_timezone=timezone;};
            bool allowed(const ImageVector & images);
    };
}
#endif