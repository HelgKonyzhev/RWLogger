#include "Logger.h"
#include "Appender.h"
#include <algorithm>

namespace RWLogger
{
	Logger::Logger(Level lvl)
		: m_level(lvl)
	{}

	void Logger::addAppender(AppenderPtr appender)
	{
		std::unique_lock<FIFOMutex> appendersLck(m_appendersMtx);
		const auto it = std::find(m_appenders.begin(), m_appenders.end(), appender);
		if(it == m_appenders.end())
			m_appenders.push_back(appender);
	}

	void Logger::removeAppender(AppenderPtr appender)
	{
		std::unique_lock<FIFOMutex> appendersLck(m_appendersMtx);
		const auto it = std::find(m_appenders.begin(), m_appenders.end(), appender);
		if(it != m_appenders.end())
			m_appenders.erase(it);
	}

	void Logger::logImpl(Level lvl, const std::string& message)
	{
		if(m_level.load() > lvl)
			return;

		const auto eventTime = std::chrono::system_clock::now();
		std::unique_lock<FIFOMutex> appendersLck(m_appendersMtx);
		for(const auto& appender: m_appenders)
			appender->append({ lvl, message, eventTime, std::this_thread::get_id() });
	}
}
