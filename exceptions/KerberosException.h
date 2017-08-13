//
//  Class: KerberosException
//  Description: Exceptions for the kerberos class.
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

#ifndef __KerberosException_H_INCLUDED__   // if KerberosException.h hasn't been included yet...
#define __KerberosException_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "Exception.h"
#include <sstream>

namespace kerberos
{
	class KerberosException : public Exception
	{
		public:
			KerberosException(const char * msg):message(msg){};
		 	virtual const char* what() const throw()
		 	{
		 		char * output = new char[300];
        strcpy (output, "Kerberos : General : A problem occured : ");
				strcat (output, message);
		 		return output;
		 	};
		private:
		 const char * message;
	};

	class KerberosFactoryCouldNotCreateException : public Exception
	{
		public:
			KerberosFactoryCouldNotCreateException(const char * message):message(message){};
		 	virtual const char * what() const throw()
		 	{
		 			char * output = new char[300];
          strcpy (output, "Kerberos : Factory : Could not create an instance of message: ");
          strcat (output, message);
          return output;
		 	};
		 private:
		 	const char * message;
	};

    class KerberosCouldNotOpenCamera  : public Exception
    {
        public:
            KerberosCouldNotOpenCamera(const char * message):message(message){};
            virtual const char* what() const throw()
            {
                char * output = new char[300];
                strcpy (output, "Kerberos : Capture : Could not open capture: ");
                strcat (output, message);
                return output;
            };
        private:
            const char * message;
    };

		class KerberosCouldNotGrabFromCamera  : public Exception
    {
        public:
            KerberosCouldNotGrabFromCamera(const char * message):message(message){};
            virtual const char* what() const throw()
            {
                char * output = new char[300];
                strcpy (output, "Kerberos : Capture : Could not grab from capture: ");
                strcat (output, message);
                return output;
            };
        private:
            const char * message;
    };
}
#endif
