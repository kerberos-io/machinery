//
//  Class: Exception
//  Description: Defines exception wrapper
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
//  https://doc.kerberos.io/license.
//
/////////////////////////////////////////////////////

#ifndef __Exception_H_INCLUDED__   // if Exception.h hasn't been included yet...
#define __Exception_H_INCLUDED__   // #define this so the compiler knows it has been included

#include <exception>
#include <cstring>

namespace kerberos
{
	class Exception : public std::exception
	{
		public:
	 		virtual const char* what() const throw() = 0;
	};
}
#endif