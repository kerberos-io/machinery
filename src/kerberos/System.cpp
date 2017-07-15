#include "System.h"

namespace kerberos
{
    std::string System::getHostname()
    {
        std::string hostname = helper::GetStdoutFromCommand("hostname");
        hostname = helper::removeUnwantedChars(hostname);

        return hostname;
    }

    std::string System::getDiskPercentage(std::string partition)
    {
        std::string percentage = helper::GetStdoutFromCommand("echo $(df -h | grep /dev/" + partition + " | head -1 | awk -F' ' '{ print $5/1 }' | tr ['%'] [\"0\"])");
        percentage = helper::removeUnwantedChars(percentage);

        return percentage;
    }

    std::string System::getTemperature()
    {
        std::string temperature = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            temperature = helper::GetStdoutFromCommand("vcgencmd measure_temp");
            temperature = helper::removeUnwantedChars(temperature);
        }

        return temperature;
    }

    std::string System::getWifiSSID()
    {
        std::string ssid = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            ssid = helper::GetStdoutFromCommand("iwconfig wlan0 | grep ESSID");
            ssid = helper::removeUnwantedChars(ssid);
        }

        return ssid;
    }

    std::string System::getWifiStrength()
    {
        std::string strength = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            strength = helper::GetStdoutFromCommand("iwconfig wlan0 | grep Quality");
            strength = helper::removeUnwantedChars(strength);
        }

        return strength;
    }

    std::string System::getBoard()
    {
        std::string board = "";

        if(RUNNING_ON_A_RASPBERRYPI)
        {
            board = helper::GetStdoutFromCommand("[ -f /etc/board ] && cat /etc/board");
            board = helper::removeUnwantedChars(board);
        }

        return board;
    }

    std::string System::isDocker()
    {
        std::string isDocker = helper::GetStdoutFromCommand("[ -f /.dockerenv ] && echo true || echo false");
        isDocker = helper::removeUnwantedChars(isDocker);

        return isDocker;
    }

    std::string System::isKiOS()
    {
        std::string isKiOS = helper::GetStdoutFromCommand("[ -f /etc/board ] && echo true || echo false");
        isKiOS = helper::removeUnwantedChars(isKiOS);

        return isKiOS;
    }

    std::string System::getUptime()
    {
        std::string uptime = helper::GetStdoutFromCommand("uptime");
        uptime = helper::removeUnwantedChars(uptime);

        return uptime;
    }

    std::string System::getCPUid()
    {
        std::string uptime = helper::GetStdoutFromCommand("[ -f /proc/cpuinfo ] && cat /proc/cpuinfo | grep Serial");
        uptime = helper::removeUnwantedChars(uptime);

        return uptime;
    }

    std::string System::getNumberOfFiles(std::string & directory)
    {
        std::string numberOfFiles = helper::GetStdoutFromCommand("ls -al " + directory + " | wc -l");
        numberOfFiles = helper::removeUnwantedChars(numberOfFiles);

        return numberOfFiles;
    }
}
