#ifndef _SM_IO_MTFILELOG
#define _SM_IO_MTFILELOG
#include "AnyType.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListStringNN.h"
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
		Data::ArrayListNative<Data::Timestamp> dateList;
		Data::ArrayListStringNN msgList;
		Sync::Event evt;

		IO::LogHandler::LogType logStyle;
		IO::LogHandler::LogGroup groupStyle;
		Int32 lastVal;
		NN<Text::UTF8Writer> log;
		NN<IO::BufferedOutputStream> cstm;
		NN<IO::FileStream> fileStm;
		UnsafeArray<const UTF8Char> dateFormat;
		NN<Text::String> fileName;
		Optional<Text::String> extName;
		Bool closed;
		Bool running;
		Bool hasNewFile;

		UnsafeArray<UTF8Char> GetNewName(UnsafeArray<UTF8Char> buff, NN<Data::DateTimeUtil::TimeValue> logTime, UInt32 nanosec, OptOut<Int32> lastVal);
		void WriteArr(UnsafeArray<NN<Text::String>> msgArr, UnsafeArray<Data::Timestamp> dateArr, UOSInt arrCnt);
		static UInt32 __stdcall FileThread(AnyType userObj);
		void Init(LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat);
	public:
		MTFileLog(NN<Text::String> fileName, LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat);
		MTFileLog(Text::CStringNN fileName, LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat);
		virtual ~MTFileLog();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);

		Bool HasNewFile();
		UnsafeArray<UTF8Char> GetLastFileName(UnsafeArray<UTF8Char> sbuff);
	};
}
#endif
