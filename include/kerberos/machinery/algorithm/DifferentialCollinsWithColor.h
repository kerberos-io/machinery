//
//  Class: DifferentialCollinsWithColor
//  Description: An algorithm to detect motion using the algorithm of colins et al.
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

#ifndef __DifferentialCollinsWithColor_H_INCLUDED__   // if DifferentialCollinsWithColor.h hasn't been included yet...
#define __DifferentialCollinsWithColor_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/algorithm/Algorithm.h"

namespace kerberos
{
    char DifferentialCollinsWithColorName[] = "DifferentialCollinsWithColor";
    class DifferentialCollinsWithColor : public AlgorithmCreator<DifferentialCollinsWithColorName, DifferentialCollinsWithColor>
    {
        private:
            int m_threshold;
            Image m_erodeKernel;
            Image h_d1, h_d2;

        public:
            DifferentialCollinsWithColor(){}
            void setup(const StringMap & settings);
        
            void setErodeKernel(int width, int height);
            void setThreshold(int threshold);
            void initialize(ImageVector & images);
            Image evaluate(ImageVector & images, JSON & data);
    };
}
#endif