#include "machinery/condition/Enabled.h"

namespace kerberos
{
    void Enabled::setup(const StringMap & settings)
    {
        Condition::setup(settings);
        
        bool enabled = false;
        if(settings.at("conditions.Enabled.active") == "true")
        {
            enabled = true;
        }

        setEnabled(enabled);
        setDelay(std::atoi(settings.at("conditions.Enabled.delay").c_str()));
    }

    bool Enabled::allowed(const ImageVector & images)
    {
        if(!isEnabled())
        {
            BINFO << "Condition: not enabled.";
            usleep(getDelay()*1000);
        }
        
        return m_enabled;
    }
}