#include "machinery/io/IoMQTT.h"

namespace kerberos
{
    void IoMQTT::setup(const StringMap & settings)
    {
        Io::setup(settings);;

        // -------------
        // Set throttler

        //throttle.setRate(std::stoi(settings.at("ios.MQTT.throttler")));
    }

    bool IoMQTT::save(Image & image, JSON & data)
    {
        if(throttle.canExecute())
        {
            return true;
        }

        return true;
    }
}
