#include "TestsRunner.h"
#include "FileAppender.h"
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <cstring>

struct FilesAppenderTest : public Test
{
	using Test::Test;

	void run() noexcept(false) override
	{
		using namespace RWLogger;

		Logger logger("FileLogger");
		auto appender = logger.addAppender<FileAppender>("result", false, true, TestsRunner::testFormat);

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
		witnessOut << "TRACE FileLogger - Trace message" << std::endl;
		witnessOut << "DEBUG FileLogger - Debug message" << std::endl;
		witnessOut << "INFO  FileLogger - Info message"  << std::endl;
		witnessOut << "WARN  FileLogger - Warn message"  << std::endl;
		witnessOut << "ERROR FileLogger - Error message" << std::endl;
		witnessOut << "FATAL FileLogger - Fatal message" << std::endl;
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

REGISTER_TEST(FilesAppenderTest);
