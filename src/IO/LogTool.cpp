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
#include "Manage/Process.h"
#include "Manage/StackTracer.h"
#include "Manage/SymbolResolver.h"
#include "Manage/ThreadInfo.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

void IO::LogTool::HandlerClose()
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = this->hdlrArr.GetCount();
	Data::Timestamp ts = Data::Timestamp::Now();
	while (i-- > 0)
	{
		this->hdlrArr.GetItem(i)->LogAdded(ts, CSTR("End logging normally"),  (IO::LogHandler::LogLevel)0);
		this->hdlrArr.GetItem(i)->LogClosed();
	}
}

IO::LogTool::LogTool()
{
	closed = false;
}

IO::LogTool::~LogTool()
{
	this->ClearHandlers();
}

void IO::LogTool::Close()
{
	if (closed)
		return;
	closed = true;
	this->HandlerClose();
}

void IO::LogTool::AddFileLog(NotNullPtr<Text::String> fileName, LogHandler::LogType style, LogHandler::LogGroup groupStyle, LogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite)
{
	if (closed)
		return;
	if (directWrite)
	{
		NotNullPtr<IO::FileLog> logs;
		NEW_CLASSNN(logs, IO::FileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		this->fileLogArr.Add(logs);
	}
	else
	{
		NotNullPtr<IO::MTFileLog> logs;
		NEW_CLASSNN(logs, IO::MTFileLog(fileName, style, groupStyle, dateFormat));
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
		NotNullPtr<IO::FileLog> logs;
		NEW_CLASSNN(logs, IO::FileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		this->fileLogArr.Add(logs);
	}
	else
	{
		NotNullPtr<IO::MTFileLog> logs;
		NEW_CLASSNN(logs, IO::MTFileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		this->fileLogArr.Add(logs);
	}
}

void IO::LogTool::AddLogHandler(NotNullPtr<LogHandler> hdlr, IO::LogHandler::LogLevel logLev)
{
	if (closed)
		return;
	{
		Sync::MutexUsage mutUsage(this->hdlrMut);
		this->hdlrArr.Add(hdlr);
		this->levArr.Add(logLev);
	}

	UTF8Char buff[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Data::Timestamp ts = Data::Timestamp::Now();

	sptr2 = IO::Path::GetProcessFileName(buff);
	sptr = &buff[Text::StrLastIndexOfCharC(buff, (UOSInt)(sptr2 - buff), IO::Path::PATH_SEPERATOR) + 1];
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Program "));
	sb.AppendC(sptr, (UOSInt)(sptr2 - sptr));
	sb.AppendC(UTF8STRC(" started"));
	sb.AppendC(UTF8STRC(", version: "));
	sb.AppendTSNoZone(IO::BuildTime::GetBuildTime());
	hdlr->LogAdded(ts, sb.ToCString(), (LogHandler::LogLevel)0);
}

void IO::LogTool::RemoveLogHandler(NotNullPtr<LogHandler> hdlr)
{
	if (closed)
		return;
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = this->hdlrArr.GetCount();
	while (i-- > 0)
	{
		if (this->hdlrArr.GetItem(i) == hdlr.Ptr())
		{
			this->hdlrArr.RemoveAt(i);
			this->levArr.RemoveAt(i);
			break;
		}
	}
}

Bool IO::LogTool::HasHandler() const
{
	return this->hdlrArr.GetCount() > 0;
}

void IO::LogTool::ClearHandlers()
{
	if (!this->closed)
		this->HandlerClose();
	this->hdlrArr.Clear();
	UOSInt i = this->fileLogArr.GetCount();
	while (i-- > 0)
	{
		IO::LogHandler *logHdlr = this->fileLogArr.GetItem(i);
		DEL_CLASS(logHdlr);
	}
	this->fileLogArr.Clear();
}

void IO::LogTool::LogMessage(Text::CStringNN logMsg, LogHandler::LogLevel level)
{
	Data::Timestamp ts = Data::Timestamp::Now();
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = this->hdlrArr.GetCount();
	while (i-- > 0)
	{
		if (this->levArr.GetItem(i) >= level)
			this->hdlrArr.GetItem(i)->LogAdded(ts, logMsg, level);
	}
}

void IO::LogTool::LogStackTrace(LogHandler::LogLevel level)
{
	NotNullPtr<Manage::ThreadInfo> thread;
	if (thread.Set(Manage::ThreadInfo::GetCurrThread()))
	{
		Manage::Process proc;
		Manage::SymbolResolver addrResol(proc);
		Optional<Manage::ThreadContext> tContext = thread->GetThreadContext();
		Manage::StackTracer tracer(tContext);
		if (tracer.IsSupported())
		{
			Text::StringBuilderUTF8 sb;
			while (tracer.GoToNextLevel())
			{
				if (sb.leng > 0)
				{
					sb.Append(CSTR("\r\n\t"));
				}
				sb.AppendHex64(tracer.GetCurrentAddr());
				sb.AppendC(UTF8STRC(" "));
				addrResol.ResolveNameSB(sb, tracer.GetCurrentAddr());
			}
			this->LogMessage(sb.ToCString(), level);
		}
		tContext.Delete();
		thread.Delete();
	}

}

IO::LogHandler *IO::LogTool::GetLastFileLog()
{
	return this->fileLogArr.GetItem(this->fileLogArr.GetCount() - 1);
}
