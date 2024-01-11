#ifndef _SM_IO_SIZEROTATEFILELOG
#define _SM_IO_SIZEROTATEFILELOG
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class SizeRotateFileLog : public LogHandler
	{
	private:
		Sync::Mutex mut;
		IO::LogHandler::LogType logStyle;
		UOSInt nFiles;
		Int32 lastVal;
		NotNullPtr<Text::UTF8Writer> log;
		NotNullPtr<IO::FileStream> fileStm;
		NotNullPtr<Text::String> fileName;
		const UTF8Char *extName;
		Int64 fileSize;
		Bool closed;

		void SwapFiles();
	public:
		SizeRotateFileLog(Text::CStringNN fileName, UOSInt nFiles, Int64 fileSize);
		virtual ~SizeRotateFileLog();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);
	};
}
#endif
