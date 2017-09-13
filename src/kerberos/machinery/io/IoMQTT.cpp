#include "machinery/io/IoMQTT.h"

namespace kerberos
{
    void IoMQTT::setup(const StringMap & settings)
    {
        Io::setup(settings);;
    }

    bool IoMQTT::save(Image & image, JSON & data)
    {
        return true;
    }
}
