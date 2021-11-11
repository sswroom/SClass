#ifndef _SM_IO_LOGWRITER
#define _SM_IO_LOGWRITER
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class LogWriter : public IO::Writer
	{
	private:
		IO::LogTool *log;
		Text::StringBuilderUTF8 *sb;
		Sync::Mutex *mut;
		IO::ILogHandler::LogLevel logLev;

		void CheckLines();
	public:
		LogWriter(IO::LogTool *log, IO::ILogHandler::LogLevel logLev);
		virtual ~LogWriter();

		virtual Bool Write(const UTF8Char *str, UOSInt nChar);
		virtual Bool Write(const UTF8Char *str);
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteLine();
	};
}
#endif
