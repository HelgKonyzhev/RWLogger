#include "TestsRunner.h"
#include "Appender.h"
#include "AsyncAppender.h"
#include "Logger.h"
#include <sstream>
#include <iostream>


using namespace RWLogger;
using CallsSequence = std::stringstream;

constexpr int AppenderDelayMsec = 500;
constexpr size_t MaxQueueSize = 2;

class TestSubAppender : public Appender
{
public:
	static void testFormat(std::ostream& ostream, const LoggingEvent& event)
	{
		std::string call = std::string("SubAppender::append ") + event.message + '\n';
		ostream << call;
	}

	TestSubAppender(CallsSequence* callsSequence)
		: Appender(callsSequence, false, testFormat)
	{}

	void append(const LoggingEvent& event) override
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(AppenderDelayMsec));
		Appender::append(event);
	}
};


class TestAsyncAppender : public AsyncAppender
{
public:
	TestAsyncAppender(CallsSequence* callsSequence, size_t queueMaxSize)
		: AsyncAppender(queueMaxSize)
		, m_callsSequence(callsSequence)
	{}

	void append(const LoggingEvent& event) override
	{
		std::string call = std::string("TestAsyncAppender::append ") + event.message + '\n';
		*m_callsSequence << call;

		AsyncAppender::append(event);
	}

private:
	virtual void onQueueOverflow(size_t queueSize) override
	{
		std::string call = std::string("TestAsyncAppender::onQueueOverflow ") + std::to_string(queueSize) + '\n';
		*m_callsSequence << call;
	}

	virtual void onAppending(const LoggingEvent& event) override
	{
		std::string call = std::string("TestAsyncAppender::onAppending ") + event.message + '\n';
		*m_callsSequence << call;
	}

private:
	CallsSequence* m_callsSequence;
};


struct AsyncAppenderQueueOverflowTest : public Test
{
	using Test::Test;

	void run() noexcept(false) override
	{
		CallsSequence callsSequense;
		Logger logger;
		auto appender = std::make_shared<TestAsyncAppender>(&callsSequense, MaxQueueSize);
		appender->addAppender<TestSubAppender>(&callsSequense);
		logger.addAppender(appender);

		for(int i = 0; i < 5; i++)
			logger.info(i);

		logger.removeAppender(appender);
		appender.reset();

		size_t callNum = 0;
		while(!callsSequense.eof())
		{
			std::string call;
			std::getline(callsSequense, call);

			if(call.empty())
				continue;

			switch(callNum)
			{
			case 0:
				// TestAsyncAppender::append trying append logging event.
				ASSERT(call == "TestAsyncAppender::append 0");
				break;
			case 1:
				// Queue size < MaxQueueSize so event appended.
				ASSERT(call == "TestAsyncAppender::onAppending 0");
				break;
			case 2:
				// TestAsyncAppender::append trying append logging event.
				ASSERT(call == "TestAsyncAppender::append 1");
				break;
			case 3:
				// Queue size < MaxQueueSize so event appended.
				ASSERT(call == "TestAsyncAppender::onAppending 1");
				break;
			case 4:
				// TestAsyncAppender::append trying append logging event
				ASSERT(call == "TestAsyncAppender::append 2");
				break;
			case 5:
				// Queue overflowed, TestAsyncAppender::append waits while queue processing.
				ASSERT(call == "TestAsyncAppender::onQueueOverflow " + std::to_string(MaxQueueSize));
				break;
			case 6:
				// One event processed, TestAsyncAppender::append continued.
				// Queue processor took one event and suspended for AppenderDelayMsec in TestSubAppender::append.
				ASSERT(call == "TestAsyncAppender::onAppending 2");
				break;
			case 7:
				// TestAsyncAppender::append trying append logging event
				ASSERT(call == "TestAsyncAppender::append 3");
				break;
			case 8:
				// Queue overflowed again, TestAsyncAppender::append waits while queue processing.
				ASSERT(call == "TestAsyncAppender::onQueueOverflow " + std::to_string(MaxQueueSize));
				break;
			case 9:
				// TestSubAppender::append finished. Queue processor will process next event and again suspend for AppenderDelayMsec.
				ASSERT(call == "SubAppender::append 0");
				break;
			case 10:
				// One event processed, TestAsyncAppender::append continued.
				// Queue processor took one event from queue and suspended for AppenderDelayMsec in TestSubAppender::append.
				ASSERT(call == "TestAsyncAppender::onAppending 3");
				break;
			case 11:
				// TestAsyncAppender::append trying append logging event
				ASSERT(call == "TestAsyncAppender::append 4");
				break;
			case 12:
				// Queue overflowed again, TestAsyncAppender::append waits while queue processing.
				ASSERT(call == "TestAsyncAppender::onQueueOverflow " + std::to_string(MaxQueueSize));
				break;
			case 13:
				// TestSubAppender::append finished. Queue processor will process next event and again suspend for AppenderDelayMsec.
				ASSERT(call == "SubAppender::append 1");
				break;
			case 14:
				// One event processed, TestAsyncAppender::append continued.
				// Queue processor took one event from queue and suspended for AppenderDelayMsec in TestSubAppender::append.
				ASSERT(call == "TestAsyncAppender::onAppending 4");
				break;
			case 15:
				// No more appending events, queue is emptying with delay AppenderDelayMsec
				ASSERT(call == "SubAppender::append 2");
				break;
			case 16:
				ASSERT(call == "SubAppender::append 3");
				break;
			case 17:
				ASSERT(call == "SubAppender::append 4");
				break;
			default:
				ASSERT(false);
			}

			++callNum;
		}
	}
};

REGISTER_TEST(AsyncAppenderQueueOverflowTest);
