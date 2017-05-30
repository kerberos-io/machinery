//
//  Class: DifferentialCollins
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

#ifndef __DifferentialCollins_H_INCLUDED__   // if DifferentialCollins.h hasn't been included yet...
#define __DifferentialCollins_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/algorithm/Algorithm.h"

namespace kerberos
{
    char DifferentialCollinsName[] = "DifferentialCollins";
    class DifferentialCollins : public AlgorithmCreator<DifferentialCollinsName, DifferentialCollins>
    {
        private:
            int m_threshold;
            Image m_erodeKernel;
            Image h_d1, h_d2;
            
        public:
            DifferentialCollins(){}
            void setup(const StringMap & settings);
        
            void setErodeKernel(int width, int height);
            void setThreshold(int threshold);
            void initialize(ImageVector & images);
            Image evaluate(ImageVector & images, JSON & data);
    };
}
#endif