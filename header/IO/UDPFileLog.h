#ifndef _SM_IO_UDPFILELOG
#define _SM_IO_UDPFILELOG
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"
#include "IO/IStreamData.h"
#include "IO/UDPLog.h"

namespace IO
{
	class UDPFileLog : public IO::UDPLog
	{
	private:
		IO::IStreamData *fd;
		Data::ArrayListUInt64 *logPos;
		Data::ArrayListUInt32 *logSize;
		UInt8 *logBuff;

	public:
		UDPFileLog(IO::IStreamData *fd);
		virtual ~UDPFileLog();

		virtual UOSInt GetCount(IO::ILogHandler::LogLevel logLevel);
		virtual Bool GetLogMessage(IO::ILogHandler::LogLevel logLevel, UOSInt index, Data::DateTime *dt, Text::StringBuilderUTF *sb, Text::LineBreakType lineBreak);
		virtual Bool GetLogDescription(IO::ILogHandler::LogLevel logLevel, UOSInt index, Text::StringBuilderUTF *sb);
	};
}
#endif
