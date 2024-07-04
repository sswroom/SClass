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
		NN<Text::UTF8Writer> log;
		NN<IO::FileStream> fileStm;
		UnsafeArray<const Char> dateFormat;
		NN<Text::String> fileName;
		UnsafeArrayOpt<const UTF8Char> extName;
		Bool closed;

		UnsafeArray<UTF8Char> GetNewName(UnsafeArray<UTF8Char> buff, NN<Data::DateTimeUtil::TimeValue> logTime, UInt32 nanosec);
		void Init(LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat);
	public:
		FileLog(NN<Text::String> fileName, LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat);
		FileLog(Text::CStringNN fileName, LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat);
		virtual ~FileLog();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);
	};
}
#endif
