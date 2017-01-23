//
//  Class: OpenCVException
//  Description: Exceptions for OpenCV.
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

#ifndef __OpenCVException_H_INCLUDED__   // if OpenCVException.h hasn't been included yet...
#define __OpenCVException_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Exception.h"

namespace kerberos
{
	class OpenCVException : public Exception
	{
		public:
			OpenCVException(const char * msg):message(msg){};
		 	virtual const char* what() const throw()
		 	{
                char * output = new char[300];
                strcpy (output, "OpenCV : ");
                strcat (output, message);
                return output;
		 	};
		private:
		 	const char * message;
	};
}
#endif