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
		NotNullPtr<IO::LogTool> log;
		Text::StringBuilderUTF8 sb;
		Sync::Mutex mut;
		IO::LogHandler::LogLevel logLev;

		void CheckLines();
	public:
		LogWriter(NotNullPtr<IO::LogTool> log, IO::LogHandler::LogLevel logLev);
		virtual ~LogWriter();

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine();
	};
}
#endif
