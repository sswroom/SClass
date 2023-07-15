#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/FileUtil.h"
#include "IO/LoopFileLog.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void IO::LoopFileLog::SwapFiles()
{
	UTF8Char buff1[256];
	UTF8Char buff2[256];
	Int32 i;
	i = this->nFiles - 1;
	Text::StrConcatC(Text::StrInt32(this->fileName->ConcatTo(buff1), i), UTF8STRC(".log"));
	Text::StrConcatC(this->fileName->ConcatTo(buff2), UTF8STRC("0.tmp"));
	IO::FileUtil::RenameFile(buff1, buff2);
	while (i-- > 0)
	{
		Text::StrConcatC(Text::StrInt32(this->fileName->ConcatTo(buff1), i), UTF8STRC(".log"));
		Text::StrConcatC(Text::StrInt32(this->fileName->ConcatTo(buff2), i + 1), UTF8STRC(".log"));
		IO::FileUtil::RenameFile(buff1, buff2);
	}
	Text::StrConcatC(this->fileName->ConcatTo(buff1), UTF8STRC("0.tmp"));
	Text::StrConcatC(this->fileName->ConcatTo(buff2), UTF8STRC("0.log"));
	IO::FileUtil::RenameFile(buff1, buff2);
}

IO::LoopFileLog::LoopFileLog(Text::CString fileName, Int32 nFiles, LogType style)
{
	this->logStyle = style;
	this->nFiles = nFiles;
	this->closed = false;
	this->extName = 0;

	UTF8Char buff[256];
	UTF8Char *sptr;

	this->fileName = Text::String::New(fileName);
	SwapFiles();

	sptr = Text::StrConcatC(fileName.ConcatTo(buff), UTF8STRC("0.log"));
	NEW_CLASSNN(fileStm, IO::FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASSNN(log, Text::UTF8Writer(fileStm));
	log->WriteSignature();

	Data::DateTime dt;
	dt.SetCurrTime();
	if (logStyle == LogHandler::LogType::PerDay)
	{
		lastVal = dt.GetDay();
	}
	else if (logStyle == LogHandler::LogType::PerMonth)
	{
		lastVal = dt.GetMonth();
	}
	else if (logStyle == LogHandler::LogType::PerYear)
	{
		lastVal = dt.GetYear();
	}
	else if (logStyle == LogHandler::LogType::PerHour)
	{
		lastVal = dt.GetDay() * 24 + dt.GetHour();
	}
	else
	{
		lastVal = 0;
	}
}

IO::LoopFileLog::~LoopFileLog()
{
	this->fileName->Release();
	if (this->extName)
	{
		Text::StrDelNew(this->extName);
		this->extName = 0;
	}

	log.Delete();
	fileStm.Delete();
}

void IO::LoopFileLog::LogClosed()
{
	if (!closed)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		log->Close();
		closed = true;
	}
}
void IO::LoopFileLog::LogAdded(const Data::Timestamp &time, Text::CString logMsg, LogLevel logLev)
{
	Bool newFile = false;
	UTF8Char buff[256];
	UTF8Char *sptr;

	Sync::MutexUsage mutUsage(&this->mut);
	Data::DateTimeUtil::TimeValue tval;
	time.ToTimeValue(&tval);

	if (logStyle == LogHandler::LogType::PerDay)
	{
		if (tval.day != lastVal)
		{
			lastVal = tval.day;
			newFile = true;
		}
	}
	else if (logStyle == LogHandler::LogType::PerMonth)
	{
		if (tval.month != lastVal)
		{
			lastVal = tval.month;
			newFile = true;
		}
	}
	else if (logStyle == LogHandler::LogType::PerYear)
	{
		if (tval.year != lastVal)
		{
			lastVal = tval.year;
			newFile = true;
		}
	}
	else if (logStyle == LogHandler::LogType::PerHour)
	{
		if (lastVal != (tval.day * 24 + tval.hour))
		{
			lastVal = tval.day * 24 + tval.hour;
			newFile = true;
		}
	}

	if (newFile)
	{
		log->Close();
		log.Delete();
		fileStm.Delete();

		SwapFiles();
		sptr = Text::StrConcatC(this->fileName->ConcatTo(buff), UTF8STRC("0.log"));

		NEW_CLASSNN(fileStm, IO::FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASSNN(log, Text::UTF8Writer(fileStm));
		log->WriteSignature();

		sptr = Text::StrConcatC(time.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t"), UTF8STRC("Program running"));
		log->WriteLineC(buff, (UOSInt)(sptr - buff));
		fileStm->Flush();
	}

	if (!this->closed)
	{
		sptr = time.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t");
		Text::StringBuilderUTF8 sb;
		sb.AppendP(buff, sptr);
		sb.Append(logMsg);
		log->WriteLineC(sb.ToString(), sb.GetLength());
	}
}
