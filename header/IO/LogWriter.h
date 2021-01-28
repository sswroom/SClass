#ifndef _SM_IO_LOGWRITER
#define _SM_IO_LOGWRITER
#include "IO/IWriter.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class LogWriter : public IO::IWriter
	{
	private:
		IO::LogTool *log;
		Text::StringBuilderUTF8 *sb;
		Sync::Mutex *mut;
		IO::ILogHandler::LogLevel logLev;

	public:
		LogWriter(IO::LogTool *log, IO::ILogHandler::LogLevel logLev);
		virtual ~LogWriter();

		virtual Bool Write(const UTF8Char *str, OSInt nChar);
		virtual Bool Write(const UTF8Char *str);
		virtual Bool WriteLine(const UTF8Char *str, OSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteW(const WChar *str, OSInt nChar);
		virtual Bool WriteW(const WChar *str);
		virtual Bool WriteLineW(const WChar *str, OSInt nChar);
		virtual Bool WriteLineW(const WChar *str);
		virtual Bool WriteLine();
	};
};
#endif
