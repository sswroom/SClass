#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/FileLog.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

UTF8Char *IO::FileLog::GetNewName(UTF8Char *buff, NN<Data::DateTimeUtil::TimeValue> time, UInt32 nanosec)
{
	UTF8Char *currName;

	if (this->groupStyle == IO::LogHandler::LogGroup::NoGroup)
	{
		currName = this->fileName->ConcatTo(buff);
	}
	else if (this->groupStyle == IO::LogHandler::LogGroup::PerYear)
	{
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyy");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::LogHandler::LogGroup::PerMonth)
	{
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyyMM");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::LogHandler::LogGroup::PerDay)
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

	if (this->logStyle == IO::LogHandler::LogType::SingleFile)
	{
	}
	else if (this->logStyle == IO::LogHandler::LogType::PerHour)
	{
		lastVal = time->day * 24 + time->hour;
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMMddHH"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::LogHandler::LogType::PerDay)
	{
		lastVal = time->day;
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMMdd"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::LogHandler::LogType::PerMonth)
	{
		lastVal = time->month;
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMM"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::LogHandler::LogType::PerYear)
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

	if (this->groupStyle != IO::LogHandler::LogGroup::NoGroup)
	{
		i = this->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		this->extName = Text::StrCopyNew(&this->fileName->v[i + 1]).Ptr();
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
	Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, tval, ts.tzQhr);
	sptr = GetNewName(buff, tval, ts.inst.nanosec);

	NEW_CLASSNN(fileStm, FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASSNN(log, Text::UTF8Writer(fileStm));
	log->WriteSignature();
}

IO::FileLog::FileLog(NN<Text::String> fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	this->fileName = fileName->Clone();
	this->Init(style, groupStyle, dateFormat);
}

IO::FileLog::FileLog(Text::CStringNN fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	this->fileName = Text::String::New(fileName);
	this->Init(style, groupStyle, dateFormat);
}

IO::FileLog::~FileLog()
{
	SDEL_TEXT(this->dateFormat);
	this->fileName->Release();
	if (this->extName)
	{
		Text::StrDelNew(this->extName);
		this->extName = 0;
	}

	log.Delete();
	fileStm.Delete();
}

void IO::FileLog::LogClosed()
{
	if (!closed)
	{
		Sync::MutexUsage mutUsage(this->mut);
		log->Close();
		mutUsage.EndUse();
		closed = true;
	}
}
void IO::FileLog::LogAdded(const Data::Timestamp &time, Text::CStringNN logMsg, LogLevel logLev)
{
	Sync::MutexUsage mutUsage(this->mut);
	Bool newFile = false;
	UTF8Char buff[256];
	UTF8Char *sptr;
	Data::DateTimeUtil::TimeValue tval;
	Data::DateTimeUtil::Instant2TimeValue(time.inst.sec, time.inst.nanosec, tval, time.tzQhr);

	if (logStyle == LogHandler::LogType::PerDay)
	{
		if (tval.day != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, tval, time.inst.nanosec);
		}
	}
	else if (logStyle == LogHandler::LogType::PerMonth)
	{
		if (tval.month != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, tval, time.inst.nanosec);
		}
	}
	else if (logStyle == LogHandler::LogType::PerYear)
	{
		if (tval.year != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, tval, time.inst.nanosec);
		}
	}
	else if (logStyle == LogHandler::LogType::PerHour)
	{
		if (lastVal != (tval.day * 24 + tval.hour))
		{
			newFile = true;
			sptr = GetNewName(buff, tval, time.inst.nanosec);
		}
	}

	if (newFile)
	{
		log->Close();
		log.Delete();
		fileStm.Delete();

		NEW_CLASSNN(fileStm, IO::FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASSNN(log, Text::UTF8Writer(fileStm));
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
