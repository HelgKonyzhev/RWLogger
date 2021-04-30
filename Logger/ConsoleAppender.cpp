#include "ConsoleAppender.h"
#include <iostream>

namespace RWLogger
{
	CoutAppender::CoutAppender(bool immidiateFlush, Formatter formatter)
		: Appender(&std::cout, immidiateFlush, formatter)
	{}

	CerrAppender::CerrAppender(bool immidiateFlush, Formatter formatter)
		: Appender(&std::cerr, immidiateFlush, formatter)
	{}
}
