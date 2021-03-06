#include "TestsRunner.h"
#include "ConsoleAppender.h"
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <cstring>

struct ConsoleAppendersTest : public Test
{
	using Test::Test;

	void run() noexcept(false) override
	{
		using namespace RWLogger;

		Logger logger;
		logger.addAppender<CoutAppender>(true, TestsRunner::testFormat);

		std::ofstream resultOut("result");
		auto outBuf = std::cout.rdbuf();
		std::cout.rdbuf(resultOut.rdbuf());

		logger.trace("Trace message");
		logger.debug("Debug message");
		logger.info ("Info message");
		logger.warn ("Warn message");
		logger.error("Error message");
		logger.fatal("Fatal message");

		std::cout.rdbuf(outBuf);
		resultOut.close();

		std::ofstream witnessOut;
		witnessOut.open("witness", std::ios::out|std::ios::trunc);
		witnessOut << "TRACE Trace message" << std::endl;
		witnessOut << "DEBUG Debug message" << std::endl;
		witnessOut << "INFO  Info message"  << std::endl;
		witnessOut << "WARN  Warn message"  << std::endl;
		witnessOut << "ERROR Error message" << std::endl;
		witnessOut << "FATAL Fatal message" << std::endl;
		witnessOut.close();

		std::ifstream resultIn, witnessIn;
		resultIn.open("result", std::ios::in|std::ios::binary);
		witnessIn.open("witness", std::ios::in|std::ios::binary);

		ASSERT(!resultIn.fail() && !witnessIn.fail());

		resultIn.seekg(0, std::ios_base::end);
		auto resultSize = static_cast<int>(resultIn.tellg());
		resultIn.seekg(0, std::ios_base::beg);

		witnessIn.seekg(0, std::ios_base::end);
		auto witnessSize = static_cast<int>(witnessIn.tellg());
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

REGISTER_TEST(ConsoleAppendersTest);
