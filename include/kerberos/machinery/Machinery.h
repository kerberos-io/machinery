//
//  Class: Machinery.h
//  Description: The kerberos controller.
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

#ifndef __Machinery_H_INCLUDED__   // if Machinery.h hasn't been included yet...
#define __Machinery_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/condition/Condition.h"
#include "machinery/algorithm/Algorithm.h"
#include "machinery/heuristic/Heuristic.h"
#include "machinery/expositor/Expositor.h"
#include "machinery/io/Io.h"
#include "easylogging++.h"
#include "capture/Capture.h";

namespace kerberos
{
	class Machinery
	{
		private:
            std::vector<Condition *> m_conditions;
            Algorithm * m_algorithm;
            Expositor * m_expositor;
            Heuristic * m_heuristic;
            std::vector<Io *> m_ios;
            Capture * m_capture;

		public:
            Machinery():m_conditions(0),m_algorithm(0),m_expositor(0),m_heuristic(0),m_ios(0){};
            ~Machinery()
            {
                delete m_algorithm;
                delete m_expositor;
                delete m_heuristic;
                
                for(int i = 0; i < m_conditions.size(); i++)
                {
                    delete m_conditions[i];
                }
                
                for(int i = 0; i < m_ios.size(); i++)
                {
                    delete m_ios[i];
                }
            };
                    
            void setup(const StringMap & settings);
            void fire(JSON & data);
            void setCapture(Capture * capture){m_capture = capture;};
            void disableCapture();
            void setCondition(std::vector<Condition *> conditions){m_conditions = conditions;};
            void setAlgorithm(Algorithm * algorithm){m_algorithm = algorithm;};
            void setExpositor(Expositor * expositor){m_expositor = expositor;};
            void setHeuristic(Heuristic * heuristic){m_heuristic = heuristic;};
            void setIo(std::vector<Io *> ios){m_ios = ios;};
            bool save(Image & image, JSON & data);
                        
            void initialize(ImageVector & images);
            bool allowed(const ImageVector & images);
            void update(const ImageVector & images);
            bool detect(ImageVector & images, JSON & data);
            bool detectMotion(ImageVector & images, JSON & data);
	};
}
#endif 
