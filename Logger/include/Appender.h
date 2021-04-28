#pragma once
#include "Commons.h"
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <ostream>
#include <mutex>

namespace RWLogger
{
	class Appender
	{
	public:
		struct LoggingEvent
		{
			Level level;
			std::string message;
			std::chrono::system_clock::time_point time;
			std::thread::id threadId;
			std::string loggerName;
		};

		using Formatter = std::function<void(std::ostream& ostream, const LoggingEvent&)>;

		static const char* levelName(Level level);

		Appender(bool immidiateFlush = true, Formatter formatter = nullptr);
		Appender(std::ostream* os, bool immidiateFlush = true, Formatter formatter = nullptr);
		Appender(const Appender&) = delete;
		virtual ~Appender();

		virtual void append(const LoggingEvent& event);
		bool immidiateFlush() const { return m_immidiateFlush; }
		void format(std::ostream& ostream, const LoggingEvent& event) const;

	protected:
		void setOStream(std::ostream* os) { m_ostream = os; }
		std::ostream* ostream() const { return m_ostream; }

	private:
		std::ostream* m_ostream = nullptr;
		bool m_immidiateFlush = true;
		Formatter m_formatter;
	};
}
