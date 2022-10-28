#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/FileLog.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

UTF8Char *IO::FileLog::GetNewName(UTF8Char *buff, Data::DateTimeUtil::TimeValue *time, UInt32 nanosec)
{
	UTF8Char *currName;

	if (this->groupStyle == IO::ILogHandler::LogGroup::NoGroup)
	{
		currName = this->fileName->ConcatTo(buff);
	}
	else if (this->groupStyle == IO::ILogHandler::LogGroup::PerYear)
	{
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyy");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LogGroup::PerMonth)
	{
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyyMM");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LogGroup::PerDay)
	{
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyyMMdd");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else
	{
		currName = this->fileName->ConcatTo(buff);
	}

	if (this->logStyle == IO::ILogHandler::LogType::SingleFile)
	{
	}
	else if (this->logStyle == IO::ILogHandler::LogType::PerHour)
	{
		lastVal = time->day * 24 + time->hour;
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMMddHH"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::ILogHandler::LogType::PerDay)
	{
		lastVal = time->day;
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMMdd"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::ILogHandler::LogType::PerMonth)
	{
		lastVal = time->month;
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMM"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::ILogHandler::LogType::PerYear)
	{
		lastVal = time->year;
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyy"), UTF8STRC(".log"));
	}
	return currName;
}

void IO::FileLog::Init(LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	UTF8Char buff[256];
	UTF8Char *sptr;
	Char cbuff[256];
	if (dateFormat)
	{
		Text::StrConcatC(Text::StrConcat(cbuff, dateFormat), "\t", 1);
		this->dateFormat = Text::StrCopyNew(cbuff);
	}
	else
	{
		this->dateFormat = Text::StrCopyNew("yyyy-MM-dd HH:mm:ss\t");
	}
	this->logStyle = style;
	this->groupStyle = groupStyle;
	this->closed = false;

	UOSInt i;

	if (this->groupStyle != IO::ILogHandler::LogGroup::NoGroup)
	{
		i = this->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		this->extName = Text::StrCopyNew(&this->fileName->v[i + 1]);
	}
	else
	{
		this->extName = 0;
		i = this->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		if (i != INVALID_INDEX)
		{
			this->fileName->v[i] = 0;
			if (!IO::Path::IsDirectoryExist(this->fileName->ToCString()))
			{
				IO::Path::CreateDirectory(this->fileName->ToCString());
			}
			this->fileName->v[i] = IO::Path::PATH_SEPERATOR;
		}
	}

	Data::Timestamp ts = Data::Timestamp::Now();
	Data::DateTimeUtil::TimeValue tval;
	Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, &tval, ts.tzQhr);
	sptr = GetNewName(buff, &tval, ts.inst.nanosec);

	NEW_CLASS(fileStm, FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(log, Text::UTF8Writer(fileStm));
	log->WriteSignature();
}

IO::FileLog::FileLog(Text::String *fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	this->fileName = fileName->Clone();
	this->Init(style, groupStyle, dateFormat);
}

IO::FileLog::FileLog(Text::CString fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	this->fileName = Text::String::New(fileName);
	this->Init(style, groupStyle, dateFormat);
}

IO::FileLog::~FileLog()
{
	SDEL_TEXT(this->dateFormat);
	fileName->Release();
	fileName = 0;
	if (this->extName)
	{
		Text::StrDelNew(this->extName);
		this->extName = 0;
	}

	DEL_CLASS(log);
	log = 0;

	DEL_CLASS(fileStm);
	fileStm = 0;
}

void IO::FileLog::LogClosed()
{
	if (!closed)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		log->Close();
		mutUsage.EndUse();
		closed = true;
	}
}
void IO::FileLog::LogAdded(Data::Timestamp time, Text::CString logMsg, LogLevel logLev)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Bool newFile = false;
	UTF8Char buff[256];
	UTF8Char *sptr;
	Data::DateTimeUtil::TimeValue tval;
	Data::DateTimeUtil::Instant2TimeValue(time.inst.sec, time.inst.nanosec, &tval, time.tzQhr);

	if (logStyle == ILogHandler::LogType::PerDay)
	{
		if (tval.day != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, &tval, time.inst.nanosec);
		}
	}
	else if (logStyle == ILogHandler::LogType::PerMonth)
	{
		if (tval.month != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, &tval, time.inst.nanosec);
		}
	}
	else if (logStyle == ILogHandler::LogType::PerYear)
	{
		if (tval.year != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, &tval, time.inst.nanosec);
		}
	}
	else if (logStyle == ILogHandler::LogType::PerHour)
	{
		if (lastVal != (tval.day * 24 + tval.hour))
		{
			newFile = true;
			sptr = GetNewName(buff, &tval, time.inst.nanosec);
		}
	}

	if (newFile)
	{
		log->Close();
		DEL_CLASS(log);
		DEL_CLASS(fileStm);

		NEW_CLASS(fileStm, IO::FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(log, Text::UTF8Writer(fileStm));
		log->WriteSignature();

		sptr = Text::StrConcatC(time.ToString(buff, this->dateFormat), UTF8STRC("Program running"));
		log->WriteLineC(buff, (UOSInt)(sptr - buff));
		fileStm->Flush();
	}

	if (!this->closed)
	{
		sptr = time.ToString(buff, this->dateFormat);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(buff, (UOSInt)(sptr - buff));
		sb.Append(logMsg);
		log->WriteLineC(sb.ToString(), sb.GetLength());
	}
	mutUsage.EndUse();
}
