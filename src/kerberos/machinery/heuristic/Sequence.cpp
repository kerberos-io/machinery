#include "machinery/heuristic/Sequence.h"
#include <unistd.h>

namespace kerberos
{
    void Sequence::setup(const StringMap & settings)
    {
        setMinimumChanges(std::atoi(settings.at("heuristics.Sequence.minimumChanges").c_str()));
        setSequenceDuration(std::atoi(settings.at("heuristics.Sequence.minimumDuration").c_str()));
        setNoMotionDelayTime(std::atoi(settings.at("heuristics.Sequence.noMotionDelayTime").c_str()));
    }
    
    bool Sequence::isValid(const Image & evaluation, const ImageVector & images, JSON & data)
    {
        int numberOfChanges;
        Rectangle rectangle;
        numberOfChanges = data["numberOfChanges"].GetInt();

        static int duration = 0;
        if(numberOfChanges >= m_minimumChanges)
        {
            duration++;
            if(duration >= m_sequenceDuration)
            {
                BINFO << "Heuristic is valid; numberOfChanges: " + helper::to_string(numberOfChanges) + ", Sequence duration: " + helper::to_string(m_sequenceDuration);
                return true;
            }
        }
        else
        {
            // If some action occurred recently, we won't introduce a delay (which is equal to the duration).
            // The idea is that it's common that more events will be detected, once some duration happened.
            //The longer the duration is, the longer we will wait for new changes without introducing a delay.
            if(duration > 0)
            {
                duration--;
            }
            else
            {
                usleep(m_noMotionDelayTime*1000);
            }
        }
        
        return false;
    }
}
