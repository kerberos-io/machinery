//
//  Class: Factory
//	Description: Creates entities, entities are registered
//               on compilation ~ Dependency injection.
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

#ifndef __Factory_H_INCLUDED__   // if Factory.h hasn't been included yet...
#define __Factory_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Globals.h"
#include "Exception.hpp"
#include "Helper.h"
#include "cloud/Cloud.h"
#include "capture/Capture.h"
#include "machinery/condition/Condition.h"
#include "machinery/algorithm/Algorithm.h"
#include "machinery/expositor/Expositor.h"
#include "machinery/heuristic/Heuristic.h"
#include "machinery/io/Io.h"

namespace kerberos
{
    template <class Class>
    class Factory
    {
        private:
            Factory(){};
            ~Factory(){};
            
        public:
            static Factory * getInstance()
            {
                static Factory fact;
                return &fact;
            }
        
            typedef Class* (*Method)();
            const char * registerClass(const char * name, Method method)
            {
                std::string _name = name;
                creators[_name] = method;
                return name;
            }
        
            Class * create(std::string name)
            {
                if(creators[name] == 0)
                    throw KerberosFactoryCouldNotCreateException(name.c_str());
                return creators[name]();
            }
        
            std::vector<Class *> createMultiple(std::string name)
            {
                std::vector<Class *> classes;
                
                // parse objects
                std::vector<std::string> tokens;
                helper::tokenize(name, tokens, ",");
                
                for(int i = 0; i < tokens.size(); i++)
                {
                    if(creators[tokens[i]] == 0)
                        throw KerberosFactoryCouldNotCreateException(tokens[i].c_str());
                    classes.push_back(creators[tokens[i]]());
                }
                return classes;
            }
        
            std::map<std::string, Method> creators;
    };
    
    // --------------------------------
    // Registration of cloud classes

    template <const char * Alias, typename Class>
    const char * CloudCreator<Alias, Class>::ID = Factory<Cloud>::getInstance()->registerClass(Alias, &CloudCreator<Alias, Class>::create);

    // --------------------------------
    // Registration of capture classes

    template <const char * Alias, typename Class>
    const char * CaptureCreator<Alias, Class>::ID = Factory<Capture>::getInstance()->registerClass(Alias, &CaptureCreator<Alias, Class>::create);
    
    // ----------------------------------
    // Registration of condition classes
    
    template <const char * Alias, typename Class>
    const char * ConditionCreator<Alias, Class>::ID = Factory<Condition>::getInstance()->registerClass(Alias, &ConditionCreator<Alias, Class>::create);
    
    // ----------------------------------
    // Registration of algorithm classes
    
    template <const char * Alias, typename Class>
    const char * AlgorithmCreator<Alias, Class>::ID = Factory<Algorithm>::getInstance()->registerClass(Alias, &AlgorithmCreator<Alias, Class>::create);
    
    // ----------------------------------
    // Registration of expositor classes

    template <const char * Alias, typename Class>
    const char * ExpositorCreator<Alias, Class>::ID = Factory<Expositor>::getInstance()->registerClass(Alias, &ExpositorCreator<Alias, Class>::create);

    // ----------------------------------
    // Registration of heuristic classes

    template <const char * Alias, typename Class>
    const char * HeuristicCreator<Alias, Class>::ID = Factory<Heuristic>::getInstance()->registerClass(Alias, &HeuristicCreator<Alias, Class>::create);
    
    // ---------------------------
    // Registration of io classes
    
    template <const char * Alias, typename Class>
    const char * IoCreator<Alias, Class>::ID = Factory<Io>::getInstance()->registerClass(Alias, &IoCreator<Alias, Class>::create);
}
#endif
