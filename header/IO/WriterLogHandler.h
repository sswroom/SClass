#ifndef _SM_IO_WRITERLOGHANDLER
#define _SM_IO_WRITERLOGHANDLER
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Sync/Mutex.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class WriterLogHandler : public LogHandler
	{
	private:
		IO::Writer *writer;
		Bool toRelease;

	public:
		WriterLogHandler(IO::Writer *writer, Bool toRelease);
		virtual ~WriterLogHandler();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);
	};
}
#endif
