#include "machinery/io/Throttler.h"

namespace kerberos
{
    bool Throttler::canExecute()
    {
        int timestamp = std::stoi(helper::getTimestamp());
        return (timestamp - m_lastTimestamp >= m_rate) && (m_lastTimestamp = timestamp);
    }
}
