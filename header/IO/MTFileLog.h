#ifndef _SM_IO_MTFILELOG
#define _SM_IO_MTFILELOG
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListNN.h"
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
		Data::ArrayListNN<Text::String> msgList;
		Sync::Event evt;

		IO::LogHandler::LogType logStyle;
		IO::LogHandler::LogGroup groupStyle;
		Int32 lastVal;
		NotNullPtr<Text::UTF8Writer> log;
		NotNullPtr<IO::BufferedOutputStream> cstm;
		NotNullPtr<IO::FileStream> fileStm;
		const UTF8Char *dateFormat;
		NotNullPtr<Text::String> fileName;
		Text::String *extName;
		Bool closed;
		Bool running;
		Bool hasNewFile;

		UTF8Char *GetNewName(UTF8Char *buff, Data::DateTimeUtil::TimeValue *logTime, UInt32 nanosec, Int32 *lastVal);
		void WriteArr(NotNullPtr<Text::String> *msgArr, Data::Timestamp *dateArr, UOSInt arrCnt);
		static UInt32 __stdcall FileThread(void *userObj);
		void Init(LogType style, LogGroup groupStyle, const Char *dateFormat);
	public:
		MTFileLog(NotNullPtr<Text::String> fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		MTFileLog(Text::CString fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		virtual ~MTFileLog();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);

		Bool HasNewFile();
		UTF8Char *GetLastFileName(UTF8Char *sbuff);
	};
}
#endif
