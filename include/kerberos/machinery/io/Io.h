//
//	Class: Io
//	Description: A (IO) input-output class is 
//               responsible for saving something
//               to disk or to database.
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

#ifndef __Io_H_INCLUDED__   // if Io.h hasn't been included yet...
#define __Io_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/FileManager.h"
#include "Helper.h"
#include "easylogging++.h"
#include "capture/Capture.h";

namespace kerberos
{
    class Io
    {
        protected:
            const char * name;

        public:
            Capture * m_capture;
            virtual ~Io(){};
            virtual void setup(const StringMap & settings);
            void setCapture(Capture * capture){m_capture = capture;};
            virtual void fire(JSON & data) = 0;
            virtual void disableCapture() = 0;
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