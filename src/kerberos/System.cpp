#include "System.h"

namespace kerberos
{
    std::string removeUnwantedChars(std::string & text)
    {
        text.erase(std::remove(text.begin(), text.end(), '\n'), text.end()); // remove newlines.
        text.erase(std::remove(text.begin(), text.end(), '\t'), text.end()); // remove tabs.
        text.erase(std::remove(text.begin(), text.end(), '"'), text.end()); // remove quotes.
        text.erase(0, text.find_first_not_of(' ')); // remove prefixing spaces.
        text.erase(text.find_last_not_of(' ')+1); // remove surfixing spaces.
        return text;
    }

    std::string System::getHostname()
    {
        std::string hostname = helper::GetStdoutFromCommand("hostname");
        hostname = ::kerberos::removeUnwantedChars(hostname);

        return hostname;
    }

    std::string System::getDiskPercentage(std::string partition)
    {
        std::string percentage = helper::GetStdoutFromCommand("echo $(df -h | grep /dev/" + partition + " | head -1 | awk -F' ' '{ print $5/1 }' | tr ['%'] [\"0\"])");
        percentage = ::kerberos::removeUnwantedChars(percentage);

        return percentage;
    }

    std::string System::getTemperature()
    {
        std::string temperature = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            temperature = helper::GetStdoutFromCommand("vcgencmd measure_temp");
            temperature = ::kerberos::removeUnwantedChars(temperature);
        }

        return temperature;
    }

    std::string System::getWifiSSID()
    {
        std::string ssid = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            ssid = helper::GetStdoutFromCommand("iwconfig wlan0 | grep ESSID");
            ssid = ::kerberos::removeUnwantedChars(ssid);
        }

        return ssid;
    }

    std::string System::getWifiStrength()
    {
        std::string strength = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            strength = helper::GetStdoutFromCommand("iwconfig wlan0 | grep Link Quality");
            strength = ::kerberos::removeUnwantedChars(strength);
        }

        return strength;
    }

    std::string System::getBoard()
    {
        std::string board = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            board = helper::GetStdoutFromCommand("[ -f /etc/board ] && cat /etc/board");
            board = ::kerberos::removeUnwantedChars(board);
        }

        return board;
    }

    std::string System::isDocker()
    {
        std::string isDocker = helper::GetStdoutFromCommand("[ -f /.dockerenv ] && echo true || echo false");
        isDocker = ::kerberos::removeUnwantedChars(isDocker);

        return isDocker;
    }

    std::string System::isKiOS()
    {
        std::string isKiOS = helper::GetStdoutFromCommand("[ -f /etc/board ] && echo true || echo false");
        isKiOS = ::kerberos::removeUnwantedChars(isKiOS);

        return isKiOS;
    }

    std::string System::getUptime()
    {
        std::string uptime = helper::GetStdoutFromCommand("uptime");
        uptime = ::kerberos::removeUnwantedChars(uptime);

        return uptime;
    }

    std::string System::getCPUid()
    {
        std::string uptime = helper::GetStdoutFromCommand("[ -f /proc/cpuinfo ] && cat /proc/cpuinfo | grep Serial");
        uptime = ::kerberos::removeUnwantedChars(uptime);

        return uptime;
    }

    std::string System::getNumberOfFiles(std::string & directory)
    {
        std::string numberOfFiles = helper::GetStdoutFromCommand("ls -al " + directory + " | wc -l");
        numberOfFiles = ::kerberos::removeUnwantedChars(numberOfFiles);

        return numberOfFiles;
    }
}
