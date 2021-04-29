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
		std::condition_variable cv;
		std::atomic_bool logging{ false };
		std::vector<std::string> logSequence;

		auto f1 = std::async(std::launch::async, [&]
		{
			std::unique_lock<std::mutex> lck(mtx);
			while(!logging)
				cv.wait(lck);

			logger.trace("f1");
			logSequence.push_back("TRACE f1");
		});

		auto f2 = std::async(std::launch::async, [&]
		{
			std::unique_lock<std::mutex> lck(mtx);
			while(!logging)
				cv.wait(lck);

			logger.debug("f2");
			logSequence.push_back("DEBUG f2");
		});

		auto f3 = std::async(std::launch::async, [&]
		{
			std::unique_lock<std::mutex> lck(mtx);
			while(!logging)
				cv.wait(lck);

			logger.info("f3");
			logSequence.push_back("INFO  f3");
		});

		auto f4 = std::async(std::launch::async, [&]
		{
			std::unique_lock<std::mutex> lck(mtx);
			while(!logging)
				cv.wait(lck);

			logger.warn("f4");
			logSequence.push_back("WARN  f4");
		});

		auto f5 = std::async(std::launch::async, [&]
		{
			std::unique_lock<std::mutex> lck(mtx);
			while(!logging)
				cv.wait(lck);

			logger.error("f5");
			logSequence.push_back("ERROR f5");
		});

		auto f6 = std::async(std::launch::async, [&]
		{
			std::unique_lock<std::mutex> lck(mtx);
			while(!logging)
				cv.wait(lck);

			logger.fatal("f6");
			logSequence.push_back("FATAL f6");
		});

		logging = true;
		cv.notify_all();

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

REGISTER_TEST(MultipleThreadsTest);
