//
//  Class: IoMQTT
//  Description: A MQTT class ..
//  Created:     25/07/2015
//  Author:      ...
//  Mail:        ...
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied under
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#ifndef __IoMQTT_H_INCLUDED__   // if IoMQTT.h hasn't been included yet...
#define __IoMQTT_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"
#include "Throttler.h"

namespace kerberos
{
    char MQTTName[] = "MQTT";
    class IoMQTT : public IoCreator<MQTTName, IoMQTT>
    {
        private:
            //std::string m_private_prop;
            Throttler throttle;

        public:
            IoMQTT(){};
            virtual ~IoMQTT()
            {
                //delete ..;
            };

            // when the machinery is booting, this method will
            // be called to initialize some properties.
            void setup(const StringMap & settings);

            // When the machinery is reconfigured, it will first call this method.
            // before remove the capture device.
            void disableCapture(){};

            // Custom functions go here (or in the private scope, whatever you prefer).
            // void setPrivateProp(std::string private_prop){m_private_prop=private_prop;};
            // std::string getPrivateProp(){return m_private_prop;};

            // Actions: there are two different types of functions.
            // 1. Non-blocking - Queued: this function will not be executed immediately.
            // 2. Blocking - Real-time: this function will be executed immediately after an event occurred.

            // Queued function.
            bool save(Image & image){ return true; }; // will be executed once when the machinery is initialized.
            bool save(Image & image, JSON & data); // will be executed every time an event occured.

            // Real-time function.
            void fire(JSON & data){};
    };
}
#endif
