#ifndef _SM_IO_LOGFILE
#define _SM_IO_LOGFILE
#include "Data/DateTime.h"
#include "IO/LogTool.h"
#include "IO/ParsedObject.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	class LogFile : public IO::ParsedObject
	{
	public:
		LogFile(Text::String *sourceName);
		LogFile(const UTF8Char *sourceName);
		virtual ~LogFile();

		virtual UOSInt GetCount(IO::ILogHandler::LogLevel logLevel) = 0;
		virtual Bool GetLogMessage(IO::ILogHandler::LogLevel logLevel, UOSInt index, Data::DateTime *dt, Text::StringBuilderUTF *sb, Text::LineBreakType lineBreak) = 0;
		virtual Bool GetLogDescription(IO::ILogHandler::LogLevel logLevel, UOSInt index, Text::StringBuilderUTF *sb) = 0;

		virtual IO::ParserType GetParserType();
	};
}
#endif
