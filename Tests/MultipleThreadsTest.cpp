#include "TestsRunner.h"
#include "FileAppender.h"
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <future>

struct MultipleThreadsTest : public Test
{
	using Test::Test;

	void run() noexcept(false) override
	{
		using namespace RWLogger;

		Logger logger;
		auto appender = logger.addAppender<FileAppender>("result", false, true, TestsRunner::testFormat);

		std::mutex mtx;
		std::vector<std::string> logSequence;

		auto f1 = std::async(std::launch::async, [&]
		{
			for(int i = 0; i < 5; i++)
			{
				logger.trace("f1 ", i);

				std::unique_lock<std::mutex> lck(mtx);
				logSequence.push_back(std::string("TRACE f1 ") + std::to_string(i));
			}
		});

		auto f2 = std::async(std::launch::async, [&]
		{
			for(int i = 0; i < 5; i++)
			{
				logger.debug("f2 ", i);

				std::unique_lock<std::mutex> lck(mtx);
				logSequence.push_back(std::string("DEBUG f2 ") + std::to_string(i));
			}
		});

		auto f3 = std::async(std::launch::async, [&]
		{
			for(int i = 0; i < 5; i++)
			{
				logger.info("f3 ", i);

				std::unique_lock<std::mutex> lck(mtx);
				logSequence.push_back(std::string("INFO  f3 ") + std::to_string(i));
			}
		});

		auto f4 = std::async(std::launch::async, [&]
		{
			for(int i = 0; i < 5; i++)
			{
				logger.warn("f4 ", i);

				std::unique_lock<std::mutex> lck(mtx);
				logSequence.push_back(std::string("WARN  f4 ") + std::to_string(i));
			}
		});

		auto f5 = std::async(std::launch::async, [&]
		{
			for(int i = 0; i < 5; i++)
			{
				logger.error("f5 ", i);

				std::unique_lock<std::mutex> lck(mtx);
				logSequence.push_back(std::string("ERROR f5 ") + std::to_string(i));
			}
		});

		auto f6 = std::async(std::launch::async, [&]
		{
			for(int i = 0; i < 5; i++)
			{
				logger.fatal("f6 ", i);

				std::unique_lock<std::mutex> lck(mtx);
				logSequence.push_back(std::string("FATAL f6 ") + std::to_string(i));
			}
		});

		f1.wait();
		f2.wait();
		f3.wait();
		f4.wait();
		f5.wait();
		f6.wait();

		logger.removeAppender(appender);
		appender.reset();

		std::ofstream witnessOut;
		witnessOut.open("witness", std::ios::out|std::ios::trunc);

		for(const auto& l: logSequence)
			witnessOut << l << std::endl;
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

REGISTER_TEST(MultipleThreadsTest);
