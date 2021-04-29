#pragma once
#include "Appender.h"

namespace RWLogger
{
	/**
	* CoutAppender appends log events to stdout using a formatter specified by the user. */
	class CoutAppender : public Appender
	{
	public:
		CoutAppender(bool immidiateFlush = true, Formatter formatter = nullptr);
	};


	/**
	* CerrAppender appends log events to stderr using a formatter specified by the user. */
	class CerrAppender : public Appender
	{
	public:
		CerrAppender(bool immidiateFlush = true, Formatter formatter = nullptr);
	};
}
