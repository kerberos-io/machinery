#include "Executor.h"

namespace kerberos
{
    SequenceInterval::SequenceInterval(IntegerTypeArray & integers)
    {
        m_count = 0;
        m_times = integers[0].first;
        m_boundery = integers[1].first;
        m_increase = m_boundery / m_times;
    }
    
    bool SequenceInterval::operator()()
    {
        m_count = (m_count+1) % m_boundery;
        
        if (m_count % m_increase == 0)
        {
            return true;
        }
        return false;
    }

    TimeInterval::TimeInterval(IntegerTypeArray & integers)
    {
        
    }
    
    bool TimeInterval::operator()()
    {
        return true;
    }
}