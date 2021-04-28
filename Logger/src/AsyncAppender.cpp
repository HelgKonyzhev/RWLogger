#include "AsyncAppender.h"

namespace RWLogger
{
	AsyncAppender::AsyncAppender(SubAppenderPtr&& subAppender, size_t queueMaxSize)
		: Appender()
		, m_subAppender(std::move(subAppender))
		, m_queueMaxSize(queueMaxSize)
		, m_appenderTread([this]{ appenderRoutine(); })
	{}

	AsyncAppender::~AsyncAppender()
	{
		{
			std::unique_lock<std::mutex> lck(m_mtx);
			while(!m_queue.empty())
				m_cv.wait(lck);
		}

		m_appending = false;
		m_cv.notify_all();
		m_appenderTread.join();
	}

	void AsyncAppender::append(const LoggingEvent& event)
	{
		std::unique_lock<std::mutex> lck(m_mtx);

		if(m_queue.size() >= m_queueMaxSize)
		{
			onQueueOverflow(m_queue.size());
			while(m_queue.size() >= m_queueMaxSize)
				m_cv.wait(lck);
		}

		onAppending(event);
		const auto wasEmpty = m_queue.empty();
		m_queue.push(event);
		if(wasEmpty)
			m_cv.notify_one();
	}

	void AsyncAppender::appenderRoutine()
	{
		if(!m_subAppender)
			return;

		bool queueEmpty = true;
		do
		{
			LoggingEvent event;
			{
				std::unique_lock<std::mutex> lck(m_mtx);
				while(m_queue.empty() && m_appending)
					m_cv.wait(lck);

				queueEmpty = m_queue.empty();
				if(queueEmpty)
					continue;

				event = m_queue.front();
				m_queue.pop();
				queueEmpty = m_queue.empty();
				m_cv.notify_one();
			}

			m_subAppender->append(event);
		}
		while(m_appending || !queueEmpty);
	}
}
