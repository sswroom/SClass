#ifndef _SM_IO_SIZEROTATEFILELOG
#define _SM_IO_SIZEROTATEFILELOG
#include "IO/StreamWriter.h"
#include "IO/FileStream.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "IO/LogTool.h"

namespace IO
{
	class SizeRotateFileLog : public ILogHandler
	{
	private:
		Sync::Mutex *mut;
		IO::ILogHandler::LogType logStyle;
		Int32 nFiles;
		Int32 lastVal;
		IO::StreamWriter *log;
		IO::FileStream *fileStm;
		Text::Encoding *enc;
		const UTF8Char *fileName;
		const UTF8Char *extName;
		Int64 fileSize;
		Bool closed;

		void SwapFiles();
	public:
		SizeRotateFileLog(const UTF8Char *fileName, Int32 nFiles, Int64 fileSize);
		virtual ~SizeRotateFileLog();
		virtual void LogClosed();
		virtual void LogAdded(Data::DateTime *logTime, const UTF8Char *logMsg, LogLevel logLev);
	};
};
#endif
