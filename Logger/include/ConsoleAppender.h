#pragma once
#include "Appender.h"

namespace RWLogger
{
	class CoutAppender : public Appender
	{
	public:
		CoutAppender(bool immidiateFlush = true, Formatter formatter = nullptr);
	};


	class CerrAppender : public Appender
	{
	public:
		CerrAppender(bool immidiateFlush = true, Formatter formatter = nullptr);
	};
}
