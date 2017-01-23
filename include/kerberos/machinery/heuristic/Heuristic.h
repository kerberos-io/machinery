//
//	Class: Heuristic
//	Description: A heuristic determines if the
//			     motion is valid. It will use the processed
//               image of the evaluation algorithm and the
//               regions calculated by the expositor. Additionally
//               it can use the originale sequence of images.
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

#include "Factory.h"
#include "capture/Image.h"
#include "easylogging++.h"

#ifndef __Heuristic_H_INCLUDED__   // if Heuristic.h hasn't been included yet...
#define __Heuristic_H_INCLUDED__   // #define this so the compiler knows it has been included

namespace kerberos
{
    class Heuristic
    {
        protected:
            const char * name;

        public:
            virtual ~Heuristic(){};
            virtual void setup(const StringMap & settings);
            virtual bool isValid(const Image & evaluation, const ImageVector & images, JSON & data) = 0;
    };

    template<const char * Alias, typename Class>
    class HeuristicCreator: public Heuristic
    {
        protected:
            HeuristicCreator(){name = ID;}
            
        public:
            static Heuristic * create(){return new Class();}
            static const char * ID;
    };
}
#endif