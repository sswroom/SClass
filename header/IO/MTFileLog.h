#ifndef _SM_IO_MTFILELOG
#define _SM_IO_MTFILELOG
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListString.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Text/String.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class MTFileLog : public LogHandler
	{
	private:
		Sync::Mutex mut;
		Data::ArrayList<Data::Timestamp> dateList;
		Data::ArrayListString msgList;
		Sync::Event evt;

		IO::LogHandler::LogType logStyle;
		IO::LogHandler::LogGroup groupStyle;
		Int32 lastVal;
		Text::UTF8Writer *log;
		IO::BufferedOutputStream *cstm;
		IO::FileStream *fileStm;
		const UTF8Char *dateFormat;
		Text::String *fileName;
		Text::String *extName;
		Bool closed;
		Bool running;
		Bool hasNewFile;

		UTF8Char *GetNewName(UTF8Char *buff, Data::DateTimeUtil::TimeValue *logTime, UInt32 nanosec, Int32 *lastVal);
		void WriteArr(Text::String **msgArr, Data::Timestamp *dateArr, UOSInt arrCnt);
		static UInt32 __stdcall FileThread(void *userObj);
		void Init(LogType style, LogGroup groupStyle, const Char *dateFormat);
	public:
		MTFileLog(Text::String *fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		MTFileLog(Text::CString fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		virtual ~MTFileLog();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CString logMsg, LogLevel logLev);

		Bool HasNewFile();
		UTF8Char *GetLastFileName(UTF8Char *sbuff);
	};
}
#endif
