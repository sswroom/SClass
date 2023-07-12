#ifndef _SM_IO_FILELOG
#define _SM_IO_FILELOG
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class FileLog : public LogHandler
	{
	private:
		Sync::Mutex mut;
		IO::LogHandler::LogType logStyle;
		IO::LogHandler::LogGroup groupStyle;
		Int32 lastVal;
		Text::UTF8Writer *log;
		IO::FileStream *fileStm;
		const Char *dateFormat;
		NotNullPtr<Text::String> fileName;
		const UTF8Char *extName;
		Bool closed;

		UTF8Char *GetNewName(UTF8Char *buff, Data::DateTimeUtil::TimeValue *logTime, UInt32 nanosec);
		void Init(LogType style, LogGroup groupStyle, const Char *dateFormat);
	public:
		FileLog(NotNullPtr<Text::String> fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		FileLog(Text::CString fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		virtual ~FileLog();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CString logMsg, LogLevel logLev);
	};
}
#endif
