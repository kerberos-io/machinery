#include "cloud/Dropbox.h"

namespace kerberos {
    void Dropbox::setup(kerberos::StringMap &settings) {
        Cloud::setup(settings);

        // -------------------------
        // Initialize Dropbox credentials

        setToken(settings.at("clouds.Dropbox.token"));
        setFolder(settings.at("clouds.Dropbox.folder"));
        Py_Initialize();
        PyObject *pName = PyString_FromString("Dropbox.py");
        PyObject *pModule = PyImport_Import(pName);
        Py_DECREF(pName);
        PyObject *pDict = PyModule_GetDict(pModule);
        authorizeFunc = PyDict_GetItemString(pDict, "authorize");
        uploadFunc = PyDict_GetItemString(pDict, "upload");
    }

    void Dropbox::setToken(std::string token) {
        m_token = token;
    }

    void Dropbox::setFolder(std::string folder) {
        m_folder = folder;
    }

    PyObject *Dropbox::authorize() {
        if (authorizeFunc && PyCallable_Check(authorizeFunc)) {
            PyObject *tokenArg = PyString_FromString(m_token.c_str());
            PyObject *args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, tokenArg);
            return PyObject_CallObject(authorizeFunc, args);
        }
    }

    bool Dropbox::upload(std::string pathToImage) {
        PyObject *session = getOrCreateSession();
        if (session != Py_None && uploadFunc && PyCallable_Check(uploadFunc)) {
            PyObject *pathArg = PyString_FromString(pathToImage.c_str());
            PyObject *folderArg = PyString_FromString(m_folder.c_str());
            PyObject *args = PyTuple_New(3);
            PyTuple_SetItem(args, 0, pathArg);
            PyTuple_SetItem(args, 1, folderArg);
            PyTuple_SetItem(args, 2, session);
            return PyObject_CallObject(uploadFunc, args);
        }
        else{
            return false;
        }
    }
}