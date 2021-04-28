#pragma once
#include "Appender.h"
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace RWLogger
{
	using SubAppenderPtr = std::unique_ptr<Appender>;

	class AsyncAppender : public Appender
	{
	public:
		AsyncAppender(SubAppenderPtr&& subAppender, size_t queueMaxSize);
		~AsyncAppender() override;

		void append(const LoggingEvent& event) override;

	private:
		void appenderRoutine();
		virtual void onQueueOverflow(size_t) {}
		virtual void onAppending(const LoggingEvent&) {}

	private:
		SubAppenderPtr m_subAppender;
		size_t m_queueMaxSize;
		std::thread m_appenderTread;
		std::atomic_bool m_appending{ true };
		std::queue<LoggingEvent> m_queue;
		std::mutex m_mtx;
		std::condition_variable_any m_cv;
	};
}
