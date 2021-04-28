#pragma once
#include "Appender.h"
#include <fstream>

namespace RWLogger
{
	class FileAppender : public Appender
	{
	public:
		FileAppender(const std::string& filename, bool append, bool immidiateFlush = true, Formatter formatter = nullptr);
		~FileAppender() override;

	private:
		std::ofstream m_ofs;
	};
}
