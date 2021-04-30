#include "AsyncAppender.h"
#include <algorithm>

namespace RWLogger
{
	AsyncAppender::AsyncAppender(size_t bufferMaxSize)
		: Appender()
		, m_bufferMaxSize(bufferMaxSize)
		, m_appenderTread([this]{ appenderRoutine(); })
	{}

	AsyncAppender::~AsyncAppender()
	{
		{
			std::unique_lock<std::mutex> lck(m_bufferMtx);
			while(!m_buffer.empty())
				m_bufferCv.wait(lck);
		}

		m_appending = false;
		m_bufferCv.notify_all();
		m_appenderTread.join();
	}

	void AsyncAppender::addAppender(AppenderPtr appender)
	{
		std::unique_lock<std::mutex> appendersLck(m_appendersMtx);
		const auto it = std::find(m_appenders.begin(), m_appenders.end(), appender);
		if(it == m_appenders.end())
			m_appenders.push_back(appender);
	}

	void AsyncAppender::removeAppender(AppenderPtr appender)
	{
		std::unique_lock<std::mutex> appendersLck(m_appendersMtx);
		const auto it = std::find(m_appenders.begin(), m_appenders.end(), appender);
		if(it != m_appenders.end())
			m_appenders.erase(it);
	}

	void AsyncAppender::append(const LoggingEvent& event)
	{
		std::unique_lock<std::mutex> lck(m_bufferMtx);

		if(m_buffer.size() >= m_bufferMaxSize)
		{
			while(m_buffer.size() >= m_bufferMaxSize)
				m_bufferCv.wait(lck);
		}

		const auto wasEmpty = m_buffer.empty();
		m_buffer.push(event);
		if(wasEmpty)
			m_bufferCv.notify_one();
	}

	void AsyncAppender::appenderRoutine()
	{
		bool queueEmpty = true;
		do
		{
			LoggingEvent event;
			{
				std::unique_lock<std::mutex> queueLck(m_bufferMtx);
				while(m_buffer.empty() && m_appending)
					m_bufferCv.wait(queueLck);

				queueEmpty = m_buffer.empty();
				if(queueEmpty)
					continue;

				event = m_buffer.front();
				m_buffer.pop();
				queueEmpty = m_buffer.empty();
				m_bufferCv.notify_one();
			}

			std::unique_lock<std::mutex> appendersLck(m_appendersMtx);
			for(auto appender: m_appenders)
				appender->append(event);
		}
		while(m_appending || !queueEmpty);
	}
}
