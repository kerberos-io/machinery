//
//  Class: IoGPIO
//  Description: A GPIO class to trigger the GPIO pins of
//               the Raspberry Pi.
//  Created:     16/12/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#ifndef __IoGPIO_H_INCLUDED__   // if IoGPIO.h hasn't been included yet...
#define __IoGPIO_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"
#include "mmapGpio.h"

namespace kerberos
{
    char GPIOName[] = "GPIO";
    class IoGPIO : public IoCreator<GPIOName, IoGPIO>
    {
        private:
            mmapGpio m_gpio;
            unsigned int m_pin;
            unsigned int m_periods;
            unsigned int m_periodTime;
        
        public:
            IoGPIO(){};
            void setup(const StringMap & settings);
            void setOutputPin(unsigned int pin);
            void setInputPin(unsigned int pin);
            void setHigh();
            void setLow();
            void setPeriods(unsigned int periods);
            void setPeriodTime(unsigned int periodTime);
            bool save(Image & image){};
            bool save(Image & image, JSON & data);
    };
}
#endif