#include "TestsRunner.h"
#include <iostream>
#include <iomanip>

void TestsRunner::testFormat(std::ostream& ostream, const RWLogger::Appender::LoggingEvent& event)
{
	ostream << std::setw(5) << std::left << RWLogger::Appender::levelName(event.level) << std::setw(0)
			<< " "
			<< event.loggerName
			<< " - "
			<< event.message
			<< '\n';
};

TestsRunner::TestsRunner()
{
	for(const auto& it: TestsCollection::instance().registeredTests())
		m_tests.emplace(it.first, it.second());
}

void TestsRunner::run() noexcept(true)
{
	size_t passedCount = 0;
	size_t failedCount = 0;

	for(const auto& it: m_tests)
	{
		try
		{
			std::cout << it.first << " starting..." << std::endl;
			it.second->run();
			std::cout << it.first << " passed" << std::endl;
			++passedCount;
		}
		catch(const std::exception& e)
		{
			std::cout << it.first << " failed: " << e.what() << std::endl;
			++failedCount;
		}
		std::cout << std::endl;
	}

	std::cout << "Total tests count: " << m_tests.size() << ", passed: " << passedCount << ", failed: " << failedCount << std::endl;
}
