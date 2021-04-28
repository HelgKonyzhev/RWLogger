#include "Appender.h"
#include <sstream>
#include <iomanip>

namespace RWLogger
{
	const char* Appender::levelName(Level level)
	{
		switch(level)
		{
		case Trace:
			return "TRACE";
		case Debug:
			return "DEBUG";
		case Info:
			return "INFO ";
		case Warn:
			return "WARN ";
		case Error:
			return "ERROR";
		case Fatal:
			return "FATAL";
		default:
			return "";
		}
	}

	Appender::Appender(bool immidiateFlush, Formatter formatter)
		: m_immidiateFlush(immidiateFlush)
	{
		m_formatter = formatter ? formatter
								: [](std::ostream& ostream, const LoggingEvent& event)->void
		{
			using namespace std::chrono;
			const auto eventTimeT = system_clock::to_time_t(event.time);
			const auto msecs = duration_cast<milliseconds>(event.time - system_clock::from_time_t(eventTimeT)).count();
			const auto timeInfo = localtime(&eventTimeT);

			ostream << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S.") << std::setfill('0') << std::setw(3) << msecs
					<< " "
					<< levelName(event.level)
					<< "\t"
					<< std::hex << std::showbase << std::internal << std::setw(10) << event.threadId << std::dec << std::setfill('\0') << std::setw(0)
					<< " "
					<< event.loggerName
					<< " - "
					<< event.message
					<< '\n';
		};
	}

	Appender::Appender(std::ostream* os, bool immidiateFlush, Formatter formatter)
		: Appender(immidiateFlush, formatter)
	{
		setOStream(os);
	}

	Appender::~Appender()
	{
		if(m_ostream)
			m_ostream->flush();
	}

	void Appender::append(const LoggingEvent& event)
	{
		if(!m_ostream)
			return;

		format(*m_ostream, event);

		if(m_immidiateFlush)
			m_ostream->flush();
	}

	void Appender::format(std::ostream& ostream, const LoggingEvent &event) const
	{
		m_formatter(ostream, event);
	}	
}
