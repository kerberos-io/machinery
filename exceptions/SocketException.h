//
//  Class: SocketException
//  Description: Exceptions for TCP/IP socket.
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

#ifndef __SocketException_H_INCLUDED__   // if SocketException.h hasn't been included yet...
#define __SocketException_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Exception.h"

namespace kerberos
{
	class SocketException : public Exception
	{
		public:
			SocketException(const char * msg):message(msg){};
		 	virtual const char* what() const throw()
		 	{
                char * output = new char[300];
                strcpy (output, "TCP/IP Socket : ");
                strcat (output, message);
		 		return output;
		 	};
		private:
		 	const char * message;
	};
}
#endif