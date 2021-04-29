#pragma once
#include "Appender.h"
#include <fstream>

namespace RWLogger
{
	/**
	 * FileAppender appends log events to a file using a formatter specified by the user */
	class FileAppender : public Appender
	{
	public:
		/**
		 * Instantiate a FileAppender and open the file
		 * designated by @param filename. The opened filename will
		 * become the output destination for this appender.
		 * If the @param append parameter is true, the file will be
		 * appended to. Otherwise, the file designated by
		 * @param filename will be truncated before being opened. */
		FileAppender(const std::string& filename, bool append, bool immidiateFlush = true, Formatter formatter = nullptr);
		~FileAppender() override;

	private:
		std::ofstream m_ofs;
	};
}
