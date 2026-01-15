#ifndef _SM_IO_CYCLICLOGBUFFER
#define _SM_IO_CYCLICLOGBUFFER
#include "Data/DateTime.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class CyclicLogBuffer : public IO::LogHandler
	{
	private:
		Sync::Mutex logMut;
		UnsafeArray<UnsafeArrayOpt<UTF8Char>> logBuff;
		UIntOS *logLeng;
		UIntOS logInd;
		UIntOS buffSize;

	public:
		CyclicLogBuffer(UIntOS buffSize);
		virtual ~CyclicLogBuffer();

		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);
		virtual void LogClosed();

		void GetLogs(NN<Text::StringBuilderUTF8> sb, Text::CStringNN seperator);
	};
}
#endif
