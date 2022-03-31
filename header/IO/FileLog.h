#ifndef _SM_IO_FILELOG
#define _SM_IO_FILELOG
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class FileLog : public ILogHandler
	{
	private:
		Sync::Mutex *mut;
		IO::ILogHandler::LogType logStyle;
		IO::ILogHandler::LogGroup groupStyle;
		Int32 lastVal;
		Text::UTF8Writer *log;
		IO::FileStream *fileStm;
		const Char *dateFormat;
		Text::String *fileName;
		const UTF8Char *extName;
		Bool closed;

		UTF8Char *GetNewName(UTF8Char *buff, Data::DateTime *logTime);
		void Init(LogType style, LogGroup groupStyle, const Char *dateFormat);
	public:
		FileLog(Text::String *fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		FileLog(Text::CString fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		virtual ~FileLog();
		virtual void LogClosed();
		virtual void LogAdded(Data::DateTime *logTime, Text::CString logMsg, LogLevel logLev);
	};
}
#endif
