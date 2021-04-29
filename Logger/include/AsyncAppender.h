#pragma once
#include "Appender.h"
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace RWLogger
{
	using AppenderPtr = std::shared_ptr<Appender>;

	/**
	 * The AsyncAppender lets users log events asynchronously. It uses a
	 * bounded buffer to store logging events.
	 * The AsyncAppender will collect the events sent to it and then
	 * dispatch them to all the appenders that are attached to it. You can
	 * attach multiple appenders to an AsyncAppender.
	 * The AsyncAppender uses a separate thread to serve the events in
	 * its bounded buffer. */
	class AsyncAppender : public Appender
	{
	public:
		/**
		 * Instantiate a AsyncAppender and sets its queue max size.
		 * @param queueMaxSize means a max size of bounded buffer to store events
		 * If this size reached all appending operation will be suspended until
		 * at least one stored event is processed */
		AsyncAppender(size_t bufferMaxSize);

		/**
		 * Waits until all buffered events are processed */
		~AsyncAppender() override;

		/**
		 * Add new appender to the list of appenders of this AsyncAppender instance.
		 * If appender already in the list of appender, then it won't be added. */
		void addAppender(AppenderPtr appender);

		/**
		 * An overloaded function. Creates appender of type T and calls AsyncAppender::addAppender(AppenderPtr appender) */
		template<typename T, typename... Args>
		AppenderPtr addAppender(Args&&... args)
		{
			static_assert (std::is_base_of<Appender, T>::value, "Only objects inherited from Appender could be added");
			auto appender = std::make_shared<T>(std::forward<Args>(args)...);
			addAppender(appender);
			return appender;
		}

		/**
		 * Remove the appender passed as parameter form the list of appenders. */
		void removeAppender(AppenderPtr appender);

		void append(const LoggingEvent& event) override;

	private:
		void appenderRoutine();

		/**
		 * This function called from append function if bounded buffer is overflown.
		 * It could be used for debugging and unit test purposes.
		 * @param bufferSize means actual buffer size. */
		virtual void onQueueOverflow(size_t bufferSize) { (void)bufferSize; }

		/**
		 * This function called from append function exactly befor event stored to bounded buffer.
		 * It could be used for debugging and unit test purposes.
		 * @param event means stored event. */
		virtual void onAppending(const LoggingEvent& event) { (void)event; }

	private:
		size_t m_bufferMaxSize;
		std::vector<AppenderPtr> m_appenders;
		std::mutex m_appendersMtx;
		std::atomic_bool m_appending{ true };
		std::queue<LoggingEvent> m_buffer;
		std::mutex m_bufferMtx;
		std::condition_variable_any m_bufferCv;
		std::thread m_appenderTread;
	};
}
