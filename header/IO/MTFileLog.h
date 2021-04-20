#ifndef _SM_IO_MTFILELOG
#define _SM_IO_MTFILELOG
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class MTFileLog : public ILogHandler
	{
	private:
		Sync::Mutex *mut;
		Data::ArrayListInt64 *dateList;
		Data::ArrayListStrUTF8 *msgList;
		Sync::Event *evt;

		IO::ILogHandler::LogType logStyle;
		IO::ILogHandler::LogGroup groupStyle;
		Int32 lastVal;
		Text::UTF8Writer *log;
		IO::FileStream *fileStm;
		const Char *dateFormat;
		const UTF8Char *fileName;
		const UTF8Char *extName;
		Bool closed;
		Bool running;

		UTF8Char *GetNewName(UTF8Char *buff, Data::DateTime *logTime);
		void WriteArr(const UTF8Char **msgArr, Int64 *dateArr, UOSInt arrCnt);
		static UInt32 __stdcall FileThread(void *userObj);
	public:
		MTFileLog(const UTF8Char *fileName, LogType style, LogGroup groupStyle, const Char *dateFormat);
		virtual ~MTFileLog();
		virtual void LogClosed();
		virtual void LogAdded(Data::DateTime *logTime, const UTF8Char *logMsg, LogLevel logLev);
	};
}
#endif
