#ifndef _SM_IO_LOGTOOL
#define _SM_IO_LOGTOOL
#include "Data/ArrayListNN.h"
#include "Data/ArrayListInt32.h"
#include "Data/Timestamp.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace IO
{
	class MTFileLog;
	class LogHandler
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
		virtual ~LogHandler(){};

		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev) = 0;
		virtual void LogClosed() = 0;
	};

	class ILogger
	{
	public:
		virtual ~ILogger() {};
		virtual void LogMessage(Text::CStringNN logMsg, LogHandler::LogLevel level) = 0;
	};

	class LogTool : public ILogger
	{
	private:
		Data::ArrayListNN<IO::LogHandler> fileLogArr;
		Data::ArrayListNN<IO::LogHandler> hdlrArr;
		Data::ArrayList<IO::LogHandler::LogLevel> levArr;
		Sync::Mutex hdlrMut;
		Bool closed;
		
	public:
		LogTool();
		virtual ~LogTool();
		void Close();
		void AddFileLog(NotNullPtr<Text::String> fileName, LogHandler::LogType style, LogHandler::LogGroup groupStyle, LogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite);
		void AddFileLog(Text::CString fileName, LogHandler::LogType style, LogHandler::LogGroup groupStyle, LogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite);
		void AddLogHandler(NotNullPtr<LogHandler> hdlr, LogHandler::LogLevel logLev);
		void RemoveLogHandler(NotNullPtr<LogHandler> hdlr);
		Bool HasHandler() const;
		virtual void LogMessage(Text::CStringNN logMsg, LogHandler::LogLevel level);
		LogHandler *GetLastFileLog();
	};
}
#endif
