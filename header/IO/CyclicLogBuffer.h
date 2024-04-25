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
		UTF8Char **logBuff;
		UOSInt *logLeng;
		UOSInt logInd;
		UOSInt buffSize;

	public:
		CyclicLogBuffer(UOSInt buffSize);
		virtual ~CyclicLogBuffer();

		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);
		virtual void LogClosed();

		void GetLogs(NN<Text::StringBuilderUTF8> sb, Text::CString seperator);
	};
}
#endif
