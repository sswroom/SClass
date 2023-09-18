#ifndef _SM_IO_CONSOLELOGHANDLER
#define _SM_IO_CONSOLELOGHANDLER
#include "IO/ConsoleWriter.h"
#include "IO/LogTool.h"

namespace IO
{
	class ConsoleLogHandler : public IO::LogHandler
	{
	private:
		NotNullPtr<IO::ConsoleWriter> console;

	public:
		ConsoleLogHandler(NotNullPtr<IO::ConsoleWriter> console);
		virtual ~ConsoleLogHandler();

		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);
		virtual void LogClosed();
	};
}
#endif
