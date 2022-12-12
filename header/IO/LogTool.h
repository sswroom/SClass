#ifndef _SM_IO_LOGTOOL
#define _SM_IO_LOGTOOL
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/Timestamp.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace IO
{
	class MTFileLog;
	class ILogHandler
	{
	public:
		enum class LogLevel
		{
			Error = 1,
			ErrorDetail = 2,
			Action = 3,
			Command = 5,
			Raw = 7
		};

		enum class LogType
		{
			SingleFile,
			PerDay,
			PerMonth,
			PerYear,
			PerHour
		};

		enum class LogGroup
		{
			NoGroup,
			PerYear,
			PerMonth,
			PerDay
		};
		virtual ~ILogHandler(){};

		virtual void LogAdded(const Data::Timestamp &logTime, Text::CString logMsg, LogLevel logLev) = 0;
		virtual void LogClosed() = 0;
	};

	class ILogger
	{
	public:
		virtual ~ILogger() {};
		virtual void LogMessage(Text::CString logMsg, ILogHandler::LogLevel level) = 0;
	};

	class LogTool : public ILogger
	{
	private:
		Data::ArrayList<IO::ILogHandler*> fileLogArr;
		Data::ArrayList<IO::ILogHandler*> hdlrArr;
		Data::ArrayList<IO::ILogHandler::LogLevel> levArr;
		Sync::Mutex hdlrMut;
		Bool closed;
		
	public:
		LogTool();
		virtual ~LogTool();
		void Close();
		void AddFileLog(Text::String *fileName, ILogHandler::LogType style, ILogHandler::LogGroup groupStyle, ILogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite);
		void AddFileLog(Text::CString fileName, ILogHandler::LogType style, ILogHandler::LogGroup groupStyle, ILogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite);
		void AddLogHandler(ILogHandler *hdlr, ILogHandler::LogLevel logLev);
		void RemoveLogHandler(ILogHandler *hdlr);
		virtual void LogMessage(Text::CString logMsg, ILogHandler::LogLevel level);
		ILogHandler *GetLastFileLog();
	};
}
#endif
