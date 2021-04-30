#include "Logger.h"
#include "ConsoleAppender.h"
#include "FileAppender.h"
#include <iostream>
#include <cstring>
#include <future>

void printHelp()
{
	using namespace std;
	cout << "Usage:" << endl;
	cout << "Example [options]" << endl;
	cout << endl;
	cout << "Options:" << endl;
	cout << "-L <logging-level> = Specify logging level of the logger." << endl;
	cout << "                     Possible values: Trace, Debug, Info, Warn, Error, Fatal." << endl;
	cout << "-l <logging-level> = Specify logging level of the following message." << endl;
	cout << "                     Possible values: Trace, Debug, Info, Warn, Error, Fatal." << endl;
	cout << "                     By default Info level used." << endl;
	cout << "-m <message>       = Appending message." << endl;
	cout << "-c <out-type>      = Add console appender of <out-type> that can be either \"cout\" or \"cerr\", " << endl;
	cout << "                     that means stdout or stderr output respectively." << endl;
	cout << "-f <filename>      = Add file appender that will append to file spicified by <filename>." << endl;
	cout << "-F <filename>      = Add file appender that will first discard content and then append to file spicified by <filename>." << endl;
	cout << "-t <threads-count> = Specify number of threads that will append specified messages. Each thread will append all messages." << endl;
	cout << "                     By default one thread is used." << endl;
	cout << "-h                   Print help." << endl;
	cout << endl;
	cout << "Usage example:" << endl;
	cout << "Example.exe -L Debug -c cout -t 2 -l Debug -m \"Debug message 1\" -m \"Debug message 2\" -l Info -m \"Info message\" -l Trace -m \"Trace message\"" << endl;
	cout << endl;
	cout << "Output:" << endl;
	cout << "2021-04-30 13:06:09.096 DEBUG	0x00000001 Debug message 1" << endl;
	cout << "2021-04-30 13:06:09.096 DEBUG	0x00000002 Debug message 1" << endl;
	cout << "2021-04-30 13:06:09.096 DEBUG	0x00000001 Debug message 2" << endl;
	cout << "2021-04-30 13:06:09.096 DEBUG	0x00000002 Debug message 2" << endl;
	cout << "2021-04-30 13:06:09.097 INFO 	0x00000001 Info message" << endl;
	cout << "2021-04-30 13:06:09.097 INFO 	0x00000002 Info message" << endl;
}

void assert(bool assertion, const std::string& failureMessage)
{
	if(!assertion)
		throw std::logic_error(failureMessage);
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printHelp();
		return -1;
	}

	const auto assertIndexValid = [argc](int index)
	{
		if(index >= argc)
			throw std::logic_error("Wrong parameters");
		return index;
	};

	using namespace RWLogger;

	Level level = Info;
	Logger logger;
	std::vector<std::string> appendersNames;
	std::vector<std::pair<Level, std::string>> messages;
	size_t threadsCount = 1;

	try
	{
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-h") == 0)
			{
				printHelp();
				return 0;
			}
			else if(strcmp(argv[i], "-c") == 0)
			{
				if(strcmp(argv[assertIndexValid(++i)], "cout") == 0)
				{
					logger.addAppender<CoutAppender>();
					appendersNames.emplace_back("Cout appender");
				}
				else if(strcmp(argv[assertIndexValid(++i)], "cerr") == 0)
				{
					logger.addAppender<CerrAppender>();
					appendersNames.emplace_back("Cerr appender");
				}
				else
				{
					assert(false, "Unknown console appender type");
				}
			}
			else if(strcmp(argv[i], "-f") == 0)
			{
				logger.addAppender<FileAppender>(argv[assertIndexValid(++i)], true);
				appendersNames.push_back(std::string("Appending file appender to ") + argv[i]);
			}
			else if(strcmp(argv[i], "-F") == 0)
			{
				logger.addAppender<FileAppender>(argv[assertIndexValid(++i)], false);
				appendersNames.push_back(std::string("Discarding file appender to ") + argv[i]);
			}
			else if(strcmp(argv[i], "-l") == 0)
			{
				assertIndexValid(++i);
				if(strcmp(argv[i], "Trace") == 0)
					level = Trace;
				else if(strcmp(argv[i], "Debug") == 0)
					level = Debug;
				else if(strcmp(argv[i], "Info") == 0)
					level = Info;
				else if(strcmp(argv[i], "Warn") == 0)
					level = Warn;
				else if(strcmp(argv[i], "Error") == 0)
					level = Error;
				else if(strcmp(argv[i], "Fatal") == 0)
					level = Fatal;
				else
					assert(false, std::string("Unknown logging level: ") + argv[i]);
			}
			else if(strcmp(argv[i], "-m") == 0)
			{
				messages.emplace_back(level, argv[assertIndexValid(++i)]);
			}
			else if(strcmp(argv[i], "-t") == 0)
			{
				threadsCount = std::stoi(argv[assertIndexValid(++i)]);
				assert(threadsCount > 0, "At least one thread should be defined");
			}
			else if(strcmp(argv[i], "-L") == 0)
			{
				assertIndexValid(++i);
				if(strcmp(argv[i], "Trace") == 0)
					logger.setLevel(Trace);
				else if(strcmp(argv[i], "Debug") == 0)
					logger.setLevel(Debug);
				else if(strcmp(argv[i], "Info") == 0)
					logger.setLevel(Info);
				else if(strcmp(argv[i], "Warn") == 0)
					logger.setLevel(Warn);
				else if(strcmp(argv[i], "Error") == 0)
					logger.setLevel(Error);
				else if(strcmp(argv[i], "Fatal") == 0)
					logger.setLevel(Fatal);
				else
					assert(false, std::string("Unknown logging level: ") + argv[i]);
			}
		}
		assert(!appendersNames.empty(), "At least one appender should be configured");

//		std::cout << "Configgured: " << std::endl;
//		std::cout << "Logger level = " << Appender::levelName(logger.level()) << std::endl;
//		std::cout << "Threads count = " << threadsCount << std::endl;
//		std::cout << "Appenders = ";
//		for(size_t i = 0; i < appendersNames.size(); i++)
//			std::cout << appendersNames[i] << (i < appendersNames.size() - 1 ? ", " : "");
//		std::cout << std::endl;
//		std::cout << std::endl;

		std::vector<std::future<void>> logFutures;
		for(size_t i = 0; i < threadsCount; i++)
		{
			logFutures.emplace_back(std::async((i == 0 ? std::launch::deferred : std::launch::async), [&]()
			{
				for(const auto& it: messages)
					logger.log(it.first, it.second);
			}));
		}

		for(auto& f: logFutures)
			f.wait();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -2;
	}

	return 0;
}
