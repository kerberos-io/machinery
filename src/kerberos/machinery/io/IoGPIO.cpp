#include "machinery/io/IoGPIO.h"

namespace kerberos
{
    void IoGPIO::setup(const StringMap & settings)
    {
        Io::setup(settings);
        setOutputPin(std::atoi(settings.at("ios.GPIO.pin").c_str()));
        setPeriods(std::atoi(settings.at("ios.GPIO.periods").c_str()));
        setPeriodTime(std::atoi(settings.at("ios.GPIO.periodTime").c_str()));
    }
    
    void IoGPIO::setOutputPin(unsigned int pin)
    {
        m_pin = pin;
        m_gpio.setPinDir(m_pin, mmapGpio::OUTPUT);
    }
    
    void IoGPIO::setInputPin(unsigned int pin)
    {
        m_pin = pin;
        m_gpio.setPinDir(m_pin, mmapGpio::INPUT);
    }
    
    void IoGPIO::setHigh()
    {
        m_gpio.writePinHigh(m_pin);
    }
    
    void IoGPIO::setLow()
    {
        m_gpio.writePinLow(m_pin);
    }
    
    void IoGPIO::setPeriods(unsigned int periods)
    {
        m_periods = periods;
    }
    
    void IoGPIO::setPeriodTime(unsigned int periodTime)
    {
        m_periodTime = periodTime;
    }
    
    bool IoGPIO::save(Image & image, JSON & data)
    {
        // ------------------------
        // Trigger the GPIO pin

        BINFO << "IoGPIO: triggering GPIO pin " + helper::to_string(m_pin);
        
        for(int i = 0; i < m_periods; i++)
        {
            // ---------------------------
            // Put pin high for some time
            
            setHigh();
            usleep(m_periodTime);
            setLow();
            usleep(m_periodTime);
        }

        return true;
    }
}