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
	NEW_CLASS(this->hdlrArr, Data::ArrayList<IO::ILogHandler*>());
	NEW_CLASS(this->hdlrMut, Sync::Mutex());
	NEW_CLASS(this->levArr, Data::ArrayListInt32());
	NEW_CLASS(this->fileLogArr, Data::ArrayList<IO::ILogHandler*>());
	closed = false;
}

IO::LogTool::~LogTool()
{
	Close();
	UOSInt i = fileLogArr->GetCount();
	while (i-- > 0)
	{
		IO::ILogHandler *logHdlr = fileLogArr->GetItem(i);
		DEL_CLASS(logHdlr);
	}
	DEL_CLASS(this->fileLogArr);
	DEL_CLASS(this->levArr);
	DEL_CLASS(this->hdlrMut);
	DEL_CLASS(this->hdlrArr);
}

void IO::LogTool::Close()
{
	if (closed)
		return;
	closed = true;
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = hdlrArr->GetCount();
	Data::DateTime dt;
	dt.SetCurrTime();
	while (i-- > 0)
	{
		hdlrArr->GetItem(i)->LogAdded(&dt, (const UTF8Char*)"End logging normally", (IO::ILogHandler::LogLevel)0);
		hdlrArr->GetItem(i)->LogClosed();
	}
	mutUsage.EndUse();
}

void IO::LogTool::AddFileLog(Text::String *fileName, ILogHandler::LogType style, ILogHandler::LogGroup groupStyle, ILogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite)
{
	if (closed)
		return;
	if (directWrite)
	{
		IO::FileLog *logs;
		NEW_CLASS(logs, IO::FileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		fileLogArr->Add(logs);
	}
	else
	{
		IO::MTFileLog *logs;
		NEW_CLASS(logs, IO::MTFileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		fileLogArr->Add(logs);
	}
}

void IO::LogTool::AddFileLog(const UTF8Char *fileName, ILogHandler::LogType style, ILogHandler::LogGroup groupStyle, ILogHandler::LogLevel logLev, const Char *dateFormat, Bool directWrite)
{
	if (closed)
		return;
	if (directWrite)
	{
		IO::FileLog *logs;
		NEW_CLASS(logs, IO::FileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		fileLogArr->Add(logs);
	}
	else
	{
		IO::MTFileLog *logs;
		NEW_CLASS(logs, IO::MTFileLog(fileName, style, groupStyle, dateFormat));
		AddLogHandler(logs, logLev);
		fileLogArr->Add(logs);
	}
}

void IO::LogTool::AddLogHandler(ILogHandler *hdlr, IO::ILogHandler::LogLevel logLev)
{
	if (closed)
		return;
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrArr->Add(hdlr);
	this->levArr->Add(logLev);
	mutUsage.EndUse();

	UTF8Char buff[256];
	UTF8Char *sptr;
	Data::DateTime dt;
	dt.SetCurrTime();

	Data::DateTime dt2;
	IO::Path::GetProcessFileName(buff);
	sptr = &buff[Text::StrLastIndexOf(buff, IO::Path::PATH_SEPERATOR) + 1];
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Program ");
	sb.Append(sptr);
	sb.Append((const UTF8Char*)" started");
	IO::BuildTime::GetBuildTime(&dt2);
	sb.Append((const UTF8Char*)", version: ");
	sb.AppendDate(&dt2);
	hdlr->LogAdded(&dt, sb.ToString(), (ILogHandler::LogLevel)0);
}

void IO::LogTool::RemoveLogHandler(ILogHandler *hdlr)
{
	if (closed)
		return;
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = this->hdlrArr->GetCount();
	while (i-- > 0)
	{
		if (this->hdlrArr->GetItem(i) == hdlr)
		{
			this->hdlrArr->RemoveAt(i);
			this->levArr->RemoveAt(i);
			break;
		}
	}
	mutUsage.EndUse();
}

void IO::LogTool::LogMessage(const UTF8Char *logMsg, ILogHandler::LogLevel level)
{
	Data::DateTime dt;
	dt.SetCurrTime();
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = hdlrArr->GetCount();
	while (i-- > 0)
	{
		if (levArr->GetItem(i) >= level)
			this->hdlrArr->GetItem(i)->LogAdded(&dt, logMsg, level);
	}
	mutUsage.EndUse();
}
