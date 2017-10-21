#include "machinery/io/Throttler.h"

namespace kerberos
{
    bool Throttler::canExecute()
    {
        std::string timestamp = helper::getTimestamp();
        LINFO << "Timestamp: " << timestamp;
        return false;
    }
}
