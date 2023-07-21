#ifndef _SM_IO_UDPFILELOG
#define _SM_IO_UDPFILELOG
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "IO/UDPLog.h"

namespace IO
{
	class UDPFileLog : public IO::UDPLog
	{
	private:
		NotNullPtr<IO::StreamData> fd;
		Data::ArrayListUInt64 logPos;
		Data::ArrayListUInt32 logSize;
		Data::ByteBuffer logBuff;

	public:
		UDPFileLog(NotNullPtr<IO::StreamData> fd);
		virtual ~UDPFileLog();

		virtual UOSInt GetCount(IO::LogHandler::LogLevel logLevel) const;
		virtual Bool GetLogMessage(IO::LogHandler::LogLevel logLevel, UOSInt index, Data::Timestamp *ts, Text::StringBuilderUTF8 *sb, Text::LineBreakType lineBreak) const;
		virtual Bool GetLogDescription(IO::LogHandler::LogLevel logLevel, UOSInt index, Text::StringBuilderUTF8 *sb) const;
	};
}
#endif
