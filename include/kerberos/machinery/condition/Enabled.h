//
//  Class: Enabled
//  Description: When enabled return true
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

#ifndef __Enabled_H_INCLUDED__   // if Enabled.h hasn't been included yet...
#define __Enabled_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/condition/Condition.h"

namespace kerberos
{
    char EnabledName[] = "Enabled";
    class Enabled : public ConditionCreator<EnabledName, Enabled>
    {
        private:
            bool m_enabled;

        public:
            Enabled(){}
            void setup(const StringMap & settings);
            void setEnabled(bool active){m_enabled=active;};
            bool isEnabled(){return m_enabled;};
            bool allowed(const ImageVector & images);
    };
}
#endif