//
//	Class: HullExpositor
//  Description: Calculate a bounding rectangle that contains 
//               all the changed pixels with a hull.
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

#ifndef __HullExpositor_H_INCLUDED__   // if HullExpositor.h hasn't been included yet...
#define __HullExpositor_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/expositor/Expositor.h"
#include "tinyxml.h"

namespace kerberos
{
    char HullName[] = "Hull";
    class HullExpositor : public ExpositorCreator<HullName, HullExpositor>
    {
        typedef std::vector<Point2f> PointVector;
        
        private:
            PointVector m_points;
            
        public:
            HullExpositor(){}
            void setup(const StringMap & settings);
            void setHull(PointVector);
            PointVector & getHull(){return m_points;};
            void calculate(Image & image, JSON & data);
    };
}
#endif