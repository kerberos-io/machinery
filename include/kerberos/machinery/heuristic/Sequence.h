//
//  Class: Sequence
//  Description: The sequence heuristic will validate
//               motion as true, as it occured for x times.
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        cedric@verstraeten.io
//  Website:     www.verstraeten.io
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied under 
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#ifndef __Sequence_H_INCLUDED__   // if Sequence.h hasn't been included yet...
#define __Sequence_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/heuristic/Heuristic.h"

namespace kerberos
{
    char SequenceName[] = "Sequence";
    class Sequence : public HeuristicCreator<SequenceName, Sequence>
    {
        private:
            int m_sequenceDuration;
            int m_minimumChanges;
            int m_noMotionDelayTime;
        
        public:
            Sequence(){}
            void setup(const StringMap & settings);
            void setMinimumChanges(int changes){m_minimumChanges=changes;};
            void setNoMotionDelayTime(int delay){m_noMotionDelayTime=delay;};
            void setSequenceDuration(int duration){m_sequenceDuration=duration;};
            bool isValid(const Image & evaluation, const ImageVector & images, JSON & data);
    };
}
#endif