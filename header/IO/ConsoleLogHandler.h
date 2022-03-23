#ifndef _SM_IO_CONSOLELOGHANDLER
#define _SM_IO_CONSOLELOGHANDLER
#include "IO/ConsoleWriter.h"
#include "IO/LogTool.h"

namespace IO
{
	class ConsoleLogHandler : public IO::ILogHandler
	{
	private:
		IO::ConsoleWriter *console;

	public:
		ConsoleLogHandler(IO::ConsoleWriter *console);
		virtual ~ConsoleLogHandler();

		virtual void LogAdded(Data::DateTime *logTime, const UTF8Char *logMsg, UOSInt msgLen, LogLevel logLev);
		virtual void LogClosed();
	};
}
#endif
