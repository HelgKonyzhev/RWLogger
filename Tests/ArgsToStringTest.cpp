#include "TestsRunner.h"
#include "FileAppender.h"
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <cstring>

struct ArgsToStringTest : public Test
{
	using Test::Test;

	void run() noexcept(false) override
	{
		using namespace RWLogger;

		Logger logger;
		auto appender = logger.addAppender<FileAppender>("result", false, true, TestsRunner::testFormat);

		int intArg = 123;
		double doubleArg = 123.456;
		char charArg = 'c';
		const char* constCharArg = "const char* message";
		std::string stringArg = "string message";

		struct CustomLoggable
		{
			std::string toLogMessage()
			{
				return "loggable custom type message";
			}
		} loggableCustomArg;

		struct CustomStringConstructible
		{
			operator std::string()
			{
				return "string constructible custom type message";
			}
		} stringConstructibeCustomArg;

		struct CustomStringConstructibleAndLoggable
		{
			operator std::string()
			{
				return "string constructible custom type message";
			}

			std::string toLogMessage()
			{
				return "loggable custom type message";
			}
		} stringConstructibeAndLoggableCustomArg;

		logger.info(intArg);
		logger.info(doubleArg);
		logger.info(charArg);
		logger.info(constCharArg);
		logger.info(stringArg);
		logger.info(loggableCustomArg);
		logger.info(stringConstructibeCustomArg);
		logger.info(stringConstructibeAndLoggableCustomArg);

		logger.info(intArg
					, ", ", doubleArg
					, ", ", charArg
					, ", ", constCharArg
					, ", ", stringArg
					, ", ", loggableCustomArg
					, ", ", stringConstructibeCustomArg
					, ", ", stringConstructibeAndLoggableCustomArg);

		logger.removeAppender(appender);
		appender.reset();

		std::ofstream witnessOut;
		witnessOut.open("witness", std::ios::out|std::ios::trunc);
		witnessOut << "INFO  123" << std::endl;
		witnessOut << "INFO  123.456" << std::endl;
		witnessOut << "INFO  c"  << std::endl;
		witnessOut << "INFO  const char* message"  << std::endl;
		witnessOut << "INFO  string message" << std::endl;
		witnessOut << "INFO  loggable custom type message" << std::endl;
		witnessOut << "INFO  string constructible custom type message" << std::endl;
		witnessOut << "INFO  loggable custom type message" << std::endl;

		witnessOut << "INFO  123, 123.456, c, const char* message, string message, "
					  "loggable custom type message, string constructible custom type message, loggable custom type message"
				   << std::endl;

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

REGISTER_TEST(ArgsToStringTest);
