#ifndef _SM_IO_LOOPFILELOG
#define _SM_IO_LOOPFILELOG
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class LoopFileLog : public LogHandler
	{
	private:
		Sync::Mutex mut;
		IO::LogHandler::LogType logStyle;
		Int32 nFiles;
		Int32 lastVal;
		NN<Text::UTF8Writer> log;
		NN<IO::FileStream> fileStm;
		NN<Text::String> fileName;
		const UTF8Char *extName;
		Bool closed;

		void SwapFiles();
	public:
		LoopFileLog(Text::CStringNN fileName, Int32 nFiles, LogType style);
		virtual ~LoopFileLog();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);
	};
}
#endif
