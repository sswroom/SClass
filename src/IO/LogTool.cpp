#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "IO/BuildTime.h"
#include "IO/FileLog.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/MTFileLog.h"
#include "IO/Path.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

IO::LogTool::LogTool()
{
	closed = false;
}

IO::LogTool::~LogTool()
{
	Close();
	UOSInt i = this->fileLogArr.GetCount();
	while (i-- > 0)
	{
		IO::LogHandler *logHdlr = this->fileLogArr.GetItem(i);
		DEL_CLASS(logHdlr);
	}
}

void IO::LogTool::Close()
{
	if (closed)
		return;
	closed = true;
	Sync::MutexUsage mutUsage(&this->hdlrMut);
	UOSInt i = this->hdlrArr.GetCount();
	Data::Timestamp ts = Data::Timestamp::Now();
	while (i-- > 0)
	{
		this->hdlrArr.GetItem(i)->LogAdded(ts, CSTR("End logging normally"),  (IO::LogHandler::LogLevel)0);
		this->hdlrArr.GetItem(i)->LogClosed();
	}
}

void IO::LogTool::AddFileLog(NotNullPtr<Text::String> fileName, LogHandler::LogType style, LogHandler::LogGroup groupStyle, LogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite)
{
	if (closed)
		return;
	if (directWrite)
	{
		IO::FileLog *logs;
		NEW_CLASS(logs, IO::FileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		this->fileLogArr.Add(logs);
	}
	else
	{
		IO::MTFileLog *logs;
		NEW_CLASS(logs, IO::MTFileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		this->fileLogArr.Add(logs);
	}
}

void IO::LogTool::AddFileLog(Text::CString fileName, LogHandler::LogType style, LogHandler::LogGroup groupStyle, LogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite)
{
	if (closed)
		return;
	if (directWrite)
	{
		IO::FileLog *logs;
		NEW_CLASS(logs, IO::FileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		this->fileLogArr.Add(logs);
	}
	else
	{
		IO::MTFileLog *logs;
		NEW_CLASS(logs, IO::MTFileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		this->fileLogArr.Add(logs);
	}
}

void IO::LogTool::AddLogHandler(LogHandler *hdlr, IO::LogHandler::LogLevel logLev)
{
	if (closed)
		return;
	{
		Sync::MutexUsage mutUsage(&this->hdlrMut);
		this->hdlrArr.Add(hdlr);
		this->levArr.Add(logLev);
	}

	UTF8Char buff[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Data::Timestamp ts = Data::Timestamp::Now();

	Data::DateTime dt2;
	sptr2 = IO::Path::GetProcessFileName(buff);
	sptr = &buff[Text::StrLastIndexOfCharC(buff, (UOSInt)(sptr2 - buff), IO::Path::PATH_SEPERATOR) + 1];
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Program "));
	sb.AppendC(sptr, (UOSInt)(sptr2 - sptr));
	sb.AppendC(UTF8STRC(" started"));
	IO::BuildTime::GetBuildTime(&dt2);
	sb.AppendC(UTF8STRC(", version: "));
	sb.AppendDate(&dt2);
	hdlr->LogAdded(ts, sb.ToCString(), (LogHandler::LogLevel)0);
}

void IO::LogTool::RemoveLogHandler(LogHandler *hdlr)
{
	if (closed)
		return;
	Sync::MutexUsage mutUsage(&this->hdlrMut);
	UOSInt i = this->hdlrArr.GetCount();
	while (i-- > 0)
	{
		if (this->hdlrArr.GetItem(i) == hdlr)
		{
			this->hdlrArr.RemoveAt(i);
			this->levArr.RemoveAt(i);
			break;
		}
	}
}

void IO::LogTool::LogMessage(Text::CString logMsg, LogHandler::LogLevel level)
{
	Data::Timestamp ts = Data::Timestamp::Now();
	Sync::MutexUsage mutUsage(&this->hdlrMut);
	UOSInt i = this->hdlrArr.GetCount();
	while (i-- > 0)
	{
		if (this->levArr.GetItem(i) >= level)
			this->hdlrArr.GetItem(i)->LogAdded(ts, logMsg, level);
	}
}

IO::LogHandler *IO::LogTool::GetLastFileLog()
{
	return this->fileLogArr.GetItem(this->fileLogArr.GetCount() - 1);
}
