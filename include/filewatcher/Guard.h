//
//	Class: Guard
//	Description: Wrapper for simplefilewatcher
//				 written by James Wynn.
//  Created:     17/07/2014
//  Author:      Cédric Verstraeten
//  Mail:        hello@cedric.ws
//	Website:	 www.kerberos.io
//
//  The copyright to the computer program(s) herein
//  is the property of kerberos.io, Belgium.
//  The program(s) may be used and/or copied .
//
/////////////////////////////////////////////////////

#ifndef __Guard_H_INCLUDED__   // if Guard.h hasn't been included yet...
#define __Guard_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "FileWatcherImpl.h"

namespace FW
{
    class Guard
    {
        private:
            FileWatcher m_fw;
            void (* m_job)(const std::string &);
            WatchID m_watchID;
            std::string m_directory;
            std::string m_file;

    	public:
    		Guard(){};
    		~Guard(){};

    		void look();
            void listenTo(const std::string & directory);
    		void listenTo(const std::string & directory, const std::string & file);
    		void onChange(void (* job)(const std::string &));
            
    		void start();
    		void startLookingForNewFiles();
            void stop();
    };

    class TaskOnFileChange : public FileWatchListener
    {
        private:
            std::string m_file;
            std::string m_directory;
            void (* m_job)(const std::string &);
            
	    public:
            TaskOnFileChange(std::string directory, std::string file, void (* job)(const std::string &)):m_directory(directory),m_file(file),m_job(job){};

	       	void handleFileAction(WatchID watchid, const std::string & directory, 
	       		const std::string & file, Action action)
	       	{
	          	if((m_file == file || m_file == "") && action == Actions::Modified)
	          	{
                    #if defined(__APPLE_CC__) || defined(BSD)
                        (*m_job)(file);
                    #elif defined(__linux__)
                        (*m_job)(m_directory + "/" + file);
                    #endif
	       		}
	       	};
    };
    
    class TaskOnFileAdded : public FileWatchListener
    {
        private:
            std::string m_file;
            std::string m_directory;
            void (* m_job)(const std::string &);
            
	    public:
            TaskOnFileAdded(std::string directory, std::string file, void (* job)(const std::string &)):m_directory(directory),m_file(file),m_job(job){};

	       	void handleFileAction(WatchID watchid, const std::string & directory, 
	       		const std::string & file, Action action)
	       	{
	          	if((m_file == file || m_file == "") && action == Actions::Add)
	          	{
                    #if defined(__APPLE_CC__) || defined(BSD)
                        (*m_job)(file);
                    #elif defined(__linux__)
                        (*m_job)(m_directory + "/" + file);
                    #endif
	       		}
	       	};
    };
};
#endif