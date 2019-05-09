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
        #if FILEWATCHER_PLATFORM == FILEWATCHER_PLATFORM_KQUEUE
			std::string file = (m_file != "") ? m_directory + "/" + m_file : m_file;
		#else
			std::string file = m_file;
		#endif

		m_watchID = m_fw.addWatch(m_directory,  new TaskOnFileChange(m_directory, file, m_job));
	}
    
    void Guard::startLookingForNewFiles()
	{
        #if FILEWATCHER_PLATFORM == FILEWATCHER_PLATFORM_KQUEUE
			std::string file = (m_file != "") ? m_directory + "/" + m_file : m_file;
		#else
			std::string file = m_file;
		#endif

		m_watchID = m_fw.addWatch(m_directory,  new TaskOnFileAdded(m_directory, file, m_job));
	}

	void Guard::stop()
	{
		m_fw.removeWatch(m_watchID);
	}
}
