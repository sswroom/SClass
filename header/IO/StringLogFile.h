#ifndef _SM_IO_STRINGLOGFILE
#define _SM_IO_STRINGLOGFILE
#include "IO/LogFile.h"

namespace IO
{
	class StringLogFile : public IO::LogFile
	{
	private:
		struct LogItem
		{
			Data::Timestamp ts;
			Optional<Text::String> message;
			Optional<Text::String> desc;
		};

		Data::ArrayListNN<LogItem> items;
	public:
		StringLogFile(NN<Text::String> sourceName);
		virtual ~StringLogFile();

		virtual UOSInt GetCount(IO::LogHandler::LogLevel logLevel) const;
		virtual Bool GetLogMessage(IO::LogHandler::LogLevel logLevel, UOSInt index, OutParam<Data::Timestamp> ts, NN<Text::StringBuilderUTF8> sb, Text::LineBreakType lineBreak) const;
		virtual Bool GetLogDescription(IO::LogHandler::LogLevel logLevel, UOSInt index, NN<Text::StringBuilderUTF8> sb) const;

		UOSInt AddLog(const Data::Timestamp &ts, Text::CString message, Text::CString desc);
	};
}
#endif
