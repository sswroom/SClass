#ifndef _SM_IO_LOOPFILELOG
#define _SM_IO_LOOPFILELOG
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class LoopFileLog : public ILogHandler
	{
	private:
		Sync::Mutex mut;
		IO::ILogHandler::LogType logStyle;
		Int32 nFiles;
		Int32 lastVal;
		Text::UTF8Writer *log;
		IO::FileStream *fileStm;
		Text::String *fileName;
		const UTF8Char *extName;
		Bool closed;

		void SwapFiles();
	public:
		LoopFileLog(Text::CString fileName, Int32 nFiles, LogType style);
		virtual ~LoopFileLog();
		virtual void LogClosed();
		virtual void LogAdded(Data::DateTime *logTime, Text::CString logMsg, LogLevel logLev);
	};
}
#endif
