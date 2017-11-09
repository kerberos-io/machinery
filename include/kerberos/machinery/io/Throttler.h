//
//  Class:       Throttler
//  Description: Only execute a function within x seconds.
//  Created:     21/10/2017
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

#ifndef __Throttler_H_INCLUDED__   // if Throttler.h hasn't been included yet...
#define __Throttler_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Helper.h"
#include "easylogging++.h"

namespace kerberos
{
    class Throttler
    {
        private:
            double m_rate; // if rate is 5, than canExecute will only execute once in 5 secs.
            int m_lastTimestamp;

        public:
            Throttler():m_lastTimestamp(0),m_rate(5){};
            ~Throttler(){};

            void setRate(double rate){m_rate = rate;}
            double getRate(){return m_rate;}
            bool canExecute();
    };
}
#endif
