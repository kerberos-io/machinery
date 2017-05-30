#include "Guard.h"

namespace FW
{
	void Guard::look()
	{
		m_fw.update();
	}
    
    void Guard::listenTo(const std::string & directory)
    {
        m_directory = directory;
        m_file = "";
    }

	void Guard::listenTo(const std::string & directory, const std::string & file)
	{
		m_directory = directory;
		m_file = file;
	}

	void Guard::onChange(void (* job)(const std::string &))
	{
		m_job = job;
	}

	void Guard::start()
	{
		#if defined(__APPLE_CC__) || defined(BSD)
			std::string file = (m_file != "") ? m_directory + "/" + m_file : m_file;
		#elif defined(__linux__)
			std::string file = m_file;
		#endif

		m_watchID = m_fw.addWatch(m_directory,  new TaskOnFileChange(m_directory, file, m_job));
	}
    
    void Guard::startLookingForNewFiles()
	{
		#if defined(__APPLE_CC__) || defined(BSD)
			std::string file = (m_file != "") ? m_directory + "/" + m_file : m_file;
		#elif defined(__linux__)
			std::string file = m_file;
		#endif

		m_watchID = m_fw.addWatch(m_directory,  new TaskOnFileAdded(m_directory, file, m_job));
	}

	void Guard::stop()
	{
		m_fw.removeWatch(m_watchID);
	}
}
