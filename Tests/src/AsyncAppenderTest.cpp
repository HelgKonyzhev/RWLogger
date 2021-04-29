#include "TestsRunner.h"
#include "FileAppender.h"
#include "AsyncAppender.h"
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <cstring>

struct AsyncAppenderTest : public Test
{
	using Test::Test;

	void run() noexcept(false) override
	{
		using namespace RWLogger;

		Logger logger("AsyncLogger");
		auto appender = std::make_shared<AsyncAppender>(10);
		appender->addAppender<FileAppender>("result", false, true, TestsRunner::testFormat);
		logger.addAppender(appender);

		logger.trace("Trace message");
		logger.debug("Debug message");
		logger.info ("Info message");
		logger.warn ("Warn message");
		logger.error("Error message");
		logger.fatal("Fatal message");

		logger.removeAppender(appender);
		appender.reset();

		std::ofstream witnessOut;
		witnessOut.open("witness", std::ios::out|std::ios::trunc);
		witnessOut << "TRACE AsyncLogger - Trace message" << std::endl;
		witnessOut << "DEBUG AsyncLogger - Debug message" << std::endl;
		witnessOut << "INFO  AsyncLogger - Info message"  << std::endl;
		witnessOut << "WARN  AsyncLogger - Warn message"  << std::endl;
		witnessOut << "ERROR AsyncLogger - Error message" << std::endl;
		witnessOut << "FATAL AsyncLogger - Fatal message" << std::endl;
		witnessOut.close();

		std::ifstream resultIn, witnessIn;
		resultIn.open("result", std::ios::in|std::ios::binary);
		witnessIn.open("witness", std::ios::in|std::ios::binary);

		ASSERT(!resultIn.fail() && !witnessIn.fail());

		resultIn.seekg(0, std::ios_base::end);
		int resultSize = resultIn.tellg();
		resultIn.seekg(0, std::ios_base::beg);

		witnessIn.seekg(0, std::ios_base::end);
		int witnessSize = witnessIn.tellg();
		witnessIn.seekg(0, std::ios_base::beg);

		ASSERT(resultSize == witnessSize);

		char* resultData = new char[resultSize + 1];
		char* witnessData = new char[witnessSize + 1];
		memset(resultData, 0, resultSize * sizeof(char));
		memset(witnessData, 0, witnessSize * sizeof(char));
		resultIn.read(resultData, resultSize);
		witnessIn.read(witnessData, witnessSize);
		resultData[resultSize] = '\0';
		witnessData[witnessSize] = '\0';

		ASSERT(memcmp(resultData, witnessData, resultSize * sizeof(char)) == 0);

		delete [] resultData;
		delete [] witnessData;

		resultIn.close();
		witnessIn.close();
	}
};

REGISTER_TEST(AsyncAppenderTest);
