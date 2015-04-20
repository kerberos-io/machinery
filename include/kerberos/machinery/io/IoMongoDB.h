//
//  Class: IoMongoDB
//  Description: A mongodb class to save information
//               in mongoDB.
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

#ifndef __IoMongoDB_H_INCLUDED__   // if IoMongoDB.h hasn't been included yet...
#define __IoMongoDB_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/io/Io.h"

namespace kerberos
{
    char MongoDBName[] = "MongoDB";
    class IoMongoDB : public IoCreator<MongoDBName, IoMongoDB>
    {
        private:
            std::string m_hostname;
            std::string m_port;
            std::string m_collection;
            
        public:
            IoMongoDB(){};
            void setup(const StringMap & settings);
            void setCollection(const std::string collection){m_collection=collection;};
            bool save(Image & image){};
            bool save(Image & image, JSON & data);
    };
}
#endif