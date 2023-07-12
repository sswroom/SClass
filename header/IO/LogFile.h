#ifndef _SM_IO_LOGFILE
#define _SM_IO_LOGFILE
#include "Data/Timestamp.h"
#include "IO/LogTool.h"
#include "IO/ParsedObject.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class LogFile : public IO::ParsedObject
	{
	public:
		LogFile(NotNullPtr<Text::String> sourceName);
		virtual ~LogFile();

		virtual UOSInt GetCount(IO::LogHandler::LogLevel logLevel) const = 0;
		virtual Bool GetLogMessage(IO::LogHandler::LogLevel logLevel, UOSInt index, Data::Timestamp *ts, Text::StringBuilderUTF8 *sb, Text::LineBreakType lineBreak) const = 0;
		virtual Bool GetLogDescription(IO::LogHandler::LogLevel logLevel, UOSInt index, Text::StringBuilderUTF8 *sb) const = 0;

		virtual IO::ParserType GetParserType() const;
	};
}
#endif
