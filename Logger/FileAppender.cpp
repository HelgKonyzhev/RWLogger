#include "FileAppender.h"

namespace RWLogger
{
	FileAppender::FileAppender(const std::string& filename, bool append, bool immidiateFlush, Formatter formatter)
		: Appender(immidiateFlush, formatter)
		, m_ofs(filename, append ? std::ios::app : std::ios::trunc)
	{
		auto b = m_ofs.is_open();
		if(b)
			setOStream(&m_ofs);
	}

	FileAppender::~FileAppender()
	{
		m_ofs.close();
	}
}
