//
//	Class: Expositor
//	Description: A expositor calculates the regions of motion
//               and determines how changed pixels are counted.
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

#ifndef __Expositor_H_INCLUDED__   // if Expositor.h hasn't been included yet...
#define __Expositor_H_INCLUDED__   // #define this so the compiler knows it has been included

namespace kerberos
{
    class Expositor
    {
        typedef std::vector<Point2f> PointVector;

        protected:
            const char * name;
            
        public:
            virtual ~Expositor(){};
            virtual void setup(const StringMap & settings);
            virtual void calculate(Image & image, JSON & data) = 0;
    };
    
    template<const char * Alias, typename Class>
    class ExpositorCreator: public Expositor
    {
        protected:
            ExpositorCreator(){name = ID;}

        public:
            static Expositor * create(){return new Class();}
            static const char * ID;
    };
}
#endif