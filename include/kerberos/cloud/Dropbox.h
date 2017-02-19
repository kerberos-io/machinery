//
//  Class: Dropbox
//  Description: Simple Storage Service of Dropbox
//  Created:     17/02/2017
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

#ifndef __DROPBOX_H_INCLUDED__   // if Dropbox.h hasn't been included yet...
#define __DROPBOX_H_INCLUDED__   // #define this so the compiler knows it has been included

#include <Python.h>
#include "cloud/Cloud.h"

namespace kerberos {
    char DropboxName[] = "Dropbox";

    class Dropbox : public CloudCreator<DropboxName, Dropbox> {
    private:
        std::string m_prevToken;
        std::string m_prevFolder;
        std::string m_token;
        std::string m_folder;
        PyObject *m_session;
        PyObject *authorizeFunc;
        PyObject *uploadFunc;

    public:

        Dropbox() {
            Py_Initialize();
        };

        virtual ~Dropbox() {
            Py_Finalize();
        };

        void setup(StringMap &settings);

        void setToken(std::string token);

        void setFolder(std::string folder);

        PyObject *getOrCreateSession() {
            if (m_session != Py_None && (!m_prevToken.compare(m_token) && !m_prevFolder.compare(m_folder))) {
                m_prevToken = m_token;
                m_prevFolder = m_folder;
                m_session = authorize();
            }
            return m_session;
        }

        bool upload(std::string pathToImage);

        PyObject *authorize();
    };
}

#endif
