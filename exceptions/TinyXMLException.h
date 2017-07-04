//
//  Class: TinyXMLException
//  Description: Exceptions for the TinyXML class.
//  Created:     24/09/2014
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

#ifndef __TinyXMLException_H_INCLUDED__   // if TinyXMLException.h hasn't been included yet...
#define __TinyXMLException_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Exception.h"

namespace kerberos
{
	class TinyXMLException : public Exception
	{
		public:
		 	virtual const char * what() const throw()
		 	{
		 		char * output = new char[300];
                strcpy (output, "TinyXML : A problem occured in the TinyXML source.");
		 		return output;
		 	};
	};

	class TinyXMLOpenFileException : public Exception
	{
		public:
		 	virtual const char * what() const throw()
		 	{
		 		char * output = new char[300];
                strcpy (output, "TinyXML : could not open the xml file, maybe the file doesn't exist?");
		 		return output;
		 	};
	};
}
#endif