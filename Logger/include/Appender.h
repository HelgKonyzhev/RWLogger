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
	/**
	 * Implement this interface for your own strategies for outputting log
	 * statements. */
	class Appender
	{
	public:
		/**
		 * The internal representation of logging events. When an affirmative
		 * decision is made to log then a LoggingEvent instance
		 * is created. This instance is passed to Appender interface.
		 * This class is of concern to those wishing to extend RWLogger. */
		struct LoggingEvent
		{
			Level level;
			std::string message;
			std::chrono::system_clock::time_point time;
			std::thread::id threadId;
			std::string loggerName;
		};

		using Formatter = std::function<void(std::ostream& ostream, const LoggingEvent&)>;

		/**
		 * Returns pointer to statically allocated c-string with appropriate level name */
		static const char* levelName(Level level);

		/**
		 * This constructor creates a new appender instance and
		 * sets its immidiate flush flag and message formatter.
		 * @param immidiateFlush means that the underlying output stream
		 * will be flushed at the end of each append operation. Immediate
		 * flush is slower but ensures that each append request is actually
		 * written.
		 * @param formatter is a callable object intended to format log message
		 * from LoggingEvent. It it's nullptr then default formatter is used
		 * By default, log messages are formatted according to a template:
		 * [Date and time with milliseconds] [Logging level name]\t[Logging thread id] [Logger name] - [Message] */
		Appender(bool immidiateFlush = true, Formatter formatter = nullptr);

		/**
		 * This constructor creates a new appender instance and
		 * sets its underlying output stream, immidiate flush flag and message formatter.
		 * @param os is a pointer to output stream that should be used to write to.
		 * Appender doesn't take ownership of the pointed memory.
		 * @param immidiateFlush means that the underlying output stream
		 * will be flushed at the end of each append operation. Immediate
		 * flush is slower but ensures that each append request is actually
		 * written.
		 * @param formatter is a callable object intended to format log message
		 * from LoggingEvent. It it's nullptr then default formatter is used
		 * By default, log messages are formatted according to a template:
		 * [Date and time with milliseconds] [Logging level name]\t[Logging thread id] [Logger name] - [Message] */
		Appender(std::ostream* os, bool immidiateFlush = true, Formatter formatter = nullptr);
		Appender(const Appender&) = delete;

		/**
		 * Flushes underlying output stream if it is set */
		virtual ~Appender();

		/**
		 * This method is called by the Logger for each logging call.
		 * If the output stream exists then formats it with set up formatter
		 * and writes a log message to the output stream. */
		virtual void append(const LoggingEvent& event);

		/**
		 * Returns value of immidiateFlush flag. */
		bool immidiateFlush() const { return m_immidiateFlush; }

		/**
		 * Formats message from @param event with set up formatter
		 * and writes it to @param ostream. */
		void format(std::ostream& ostream, const LoggingEvent& event) const;

	protected:
		/**
		 * Set up underlying output stream if this wasn't possible on construction */
		void setOStream(std::ostream* os) { m_ostream = os; }

		/**
		 * Returns pointer to underlying output stream */
		std::ostream* ostream() const { return m_ostream; }

	private:
		std::ostream* m_ostream = nullptr;
		bool m_immidiateFlush = true;
		Formatter m_formatter;
	};
}
