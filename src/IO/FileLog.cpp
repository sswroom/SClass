#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/FileLog.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

UTF8Char *IO::FileLog::GetNewName(UTF8Char *buff, Data::DateTime *time)
{
	UTF8Char *currName;

	if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP)
	{
		currName = this->fileName->ConcatTo(buff);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_YEAR)
	{
		currName = time->ToString(this->fileName->ConcatTo(buff), "yyyy");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH)
	{
		currName = time->ToString(this->fileName->ConcatTo(buff), "yyyyMM");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_DAY)
	{
		currName = time->ToString(this->fileName->ConcatTo(buff), "yyyyMMdd");
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

	if (this->logStyle == IO::ILogHandler::LOG_TYPE_SINGLE_FILE)
	{
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_HOUR)
	{
		lastVal = time->GetDay() * 24 + time->GetHour();
		currName = Text::StrConcatC(time->ToString(currName, "yyyyMMddHH"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_DAY)
	{
		lastVal = time->GetDay();
		currName = Text::StrConcatC(time->ToString(currName, "yyyyMMdd"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_MONTH)
	{
		lastVal = time->GetMonth();
		currName = Text::StrConcatC(time->ToString(currName, "yyyyMM"), UTF8STRC(".log"));
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_YEAR)
	{
		lastVal = time->GetYear();
		currName = Text::StrConcatC(time->ToString(currName, "yyyy"), UTF8STRC(".log"));
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

	if (this->groupStyle != IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP)
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

	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = GetNewName(buff, &dt);

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
void IO::FileLog::LogAdded(Data::DateTime *time, Text::CString logMsg, LogLevel logLev)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Bool newFile = false;
	UTF8Char buff[256];
	UTF8Char *sptr;

	if (logStyle == ILogHandler::LOG_TYPE_PER_DAY)
	{
		if (time->GetDay() != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, time);
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_MONTH)
	{
		if (time->GetMonth() != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, time);
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_YEAR)
	{
		if (time->GetYear() != lastVal)
		{
			newFile = true;
			sptr = GetNewName(buff, time);
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_HOUR)
	{
		if (lastVal != (time->GetDay() * 24 + time->GetHour()))
		{
			newFile = true;
			sptr = GetNewName(buff, time);
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

		sptr = Text::StrConcatC(time->ToString(buff, this->dateFormat), UTF8STRC("Program running"));
		log->WriteLineC(buff, (UOSInt)(sptr - buff));
		fileStm->Flush();
	}

	if (!this->closed)
	{
		sptr = time->ToString(buff, this->dateFormat);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(buff, (UOSInt)(sptr - buff));
		sb.Append(logMsg);
		log->WriteLineC(sb.ToString(), sb.GetLength());
	}
	mutUsage.EndUse();
}
