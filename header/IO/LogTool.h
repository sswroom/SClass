#ifndef _SM_IO_LOGTOOL
#define _SM_IO_LOGTOOL
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/DateTime.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace IO
{
	class MTFileLog;
	class ILogHandler
	{
	public:
		enum LogLevel
		{
			LOG_LEVEL_ERROR = 1,
			LOG_LEVEL_ERR_DETAIL = 2,
			LOG_LEVEL_ACTION = 3,
			LOG_LEVEL_COMMAND = 5,
			LOG_LEVEL_RAW = 7
		};

		enum LogType
		{
			LOG_TYPE_SINGLE_FILE,
			LOG_TYPE_PER_DAY,
			LOG_TYPE_PER_MONTH,
			LOG_TYPE_PER_YEAR,
			LOG_TYPE_PER_HOUR
		};

		enum LogGroup
		{
			LOG_GROUP_TYPE_NO_GROUP,
			LOG_GROUP_TYPE_PER_YEAR,
			LOG_GROUP_TYPE_PER_MONTH,
			LOG_GROUP_TYPE_PER_DAY
		};
		virtual ~ILogHandler(){};

		virtual void LogAdded(Data::DateTime *logTime, const UTF8Char *logMsg, UOSInt msgLen, LogLevel logLev) = 0;
		virtual void LogClosed() = 0;
	};

	class ILogger
	{
	public:
		virtual ~ILogger() {};
		virtual void LogMessage(const UTF8Char *logMsg, ILogHandler::LogLevel level) = 0;
		virtual void LogMessageC(const UTF8Char *logMsg, UOSInt msgLen, ILogHandler::LogLevel level) = 0;
	};

	class LogTool : public ILogger
	{
	private:
		Data::ArrayList<IO::ILogHandler*> *fileLogArr;
		Data::ArrayList<IO::ILogHandler*> *hdlrArr;
		Data::ArrayListInt32 *levArr;
		Sync::Mutex *hdlrMut;
		Bool closed;
		
	public:
		LogTool();
		virtual ~LogTool();
		void Close();
		void AddFileLog(Text::String *fileName, ILogHandler::LogType style, ILogHandler::LogGroup groupStyle, ILogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite);
		void AddFileLog(const UTF8Char *fileName, ILogHandler::LogType style, ILogHandler::LogGroup groupStyle, ILogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite);
		void AddLogHandler(ILogHandler *hdlr, ILogHandler::LogLevel logLev);
		void RemoveLogHandler(ILogHandler *hdlr);
		virtual void LogMessage(const UTF8Char *logMsg, ILogHandler::LogLevel level);
		virtual void LogMessageC(const UTF8Char *logMsg, UOSInt msgLen, ILogHandler::LogLevel level);
	};
}
#endif
