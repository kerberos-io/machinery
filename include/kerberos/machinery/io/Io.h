//
//	Class: Io
//	Description: A (IO) input-output class is 
//               responsible for saving something
//               to disk or to database.
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//  Website:     www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#include "Factory.h"

#ifndef __Io_H_INCLUDED__   // if Io.h hasn't been included yet...
#define __Io_H_INCLUDED__   // #define this so the compiler knows it has been included


#include "machinery/io/FileManager.h"
#include "Helper.h"

namespace kerberos
{
    class Io
    {
        protected:
            const char * name;

        public:
            virtual ~Io(){};
            virtual void setup(const StringMap & settings);
            virtual bool save(Image & image) = 0;
            virtual bool save(Image & image, JSON & data) = 0;
    };
    
    template<const char * Alias, typename Class>
    class IoCreator: public Io
    {
        protected:
            IoCreator(){name = ID;}
            
        public:
            static Io * create(){return new Class();}
            static const char * ID; 
    };
}
#endif