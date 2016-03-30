//
//  Class: Condition
//  Description: Condition, will tell if the machinery is allowed to process.
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#include "Factory.h"
#include "capture/Image.h"
#include "easylogging++.h"

#ifndef __Condition_H_INCLUDED__   // if Condition.h hasn't been included yet...
#define __Condition_H_INCLUDED__   // #define this so the compiler knows it has been included

namespace kerberos
{
    class Condition
    {
        protected:
            const char * name;
            int m_delay;
        public:
            virtual ~Condition(){};
            virtual void setup(const StringMap & settings);
            virtual bool allowed(const ImageVector & images) = 0;
            void setDelay(int delay){m_delay=delay;};
            int getDelay(){return m_delay;};
    };
    
    template<const char * Alias, typename Class>
    class ConditionCreator: public Condition
    {
        protected:
            ConditionCreator(){name = ID;}
            
        public:
            static Condition * create(){return new Class();}
            static const char * ID;
    };
}
#endif