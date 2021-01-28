#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/FileLog.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

UTF8Char *IO::FileLog::GetNewName(UTF8Char *buff, Data::DateTime *time)
{
	UTF8Char *currName;

	if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP)
	{
		currName = Text::StrConcat(buff, this->fileName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_YEAR)
	{
		currName = time->ToString(Text::StrConcat(buff, this->fileName), "yyyy");
		if (!IO::Path::IsDirectoryExist(buff))
		{
			IO::Path::CreateDirectory(buff);
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH)
	{
		currName = time->ToString(Text::StrConcat(buff, this->fileName), "yyyyMM");
		if (!IO::Path::IsDirectoryExist(buff))
		{
			IO::Path::CreateDirectory(buff);
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_DAY)
	{
		currName = time->ToString(Text::StrConcat(buff, this->fileName), "yyyyMMdd");
		if (!IO::Path::IsDirectoryExist(buff))
		{
			IO::Path::CreateDirectory(buff);
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else
	{
		currName = Text::StrConcat(buff, this->fileName);
	}

	if (this->logStyle == IO::ILogHandler::LOG_TYPE_SINGLE_FILE)
	{
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_HOUR)
	{
		lastVal = time->GetDay() * 24 + time->GetHour();
		currName = Text::StrConcat(time->ToString(currName, "yyyyMMddHH"), (const UTF8Char*)".log");
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_DAY)
	{
		lastVal = time->GetDay();
		currName = Text::StrConcat(time->ToString(currName, "yyyyMMdd"), (const UTF8Char*)".log");
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_MONTH)
	{
		lastVal = time->GetMonth();
		currName = Text::StrConcat(time->ToString(currName, "yyyyMM"), (const UTF8Char*)".log");
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_YEAR)
	{
		lastVal = time->GetYear();
		currName = Text::StrConcat(time->ToString(currName, "yyyy"), (const UTF8Char*)".log");
	}
	return currName;
}

IO::FileLog::FileLog(const UTF8Char *fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	UTF8Char buff[256];
	Char cbuff[256];
	if (dateFormat)
	{
		Text::StrConcat(Text::StrConcat(cbuff, dateFormat), "\t");
		this->dateFormat = Text::StrCopyNew(cbuff);
	}
	else
	{
		this->dateFormat = Text::StrCopyNew("yyyy-MM-dd HH:mm:ss\t");
	}
	NEW_CLASS(mut, Sync::Mutex());
	this->logStyle = style;
	this->groupStyle = groupStyle;
	this->closed = false;

	OSInt i;

	this->fileName = Text::StrCopyNew(fileName);
	if (this->groupStyle != IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP)
	{
		i = Text::StrLastIndexOf(this->fileName, IO::Path::PATH_SEPERATOR);
		this->extName = Text::StrCopyNew(&this->fileName[i + 1]);
	}
	else
	{
		this->extName = 0;
		i = Text::StrLastIndexOf(this->fileName, IO::Path::PATH_SEPERATOR);
		if (i >= 0)
		{
			((UTF8Char*)this->fileName)[i] = 0;
			if (!IO::Path::IsDirectoryExist(this->fileName))
			{
				IO::Path::CreateDirectory(this->fileName);
			}
			((UTF8Char*)this->fileName)[i] = IO::Path::PATH_SEPERATOR;
		}
	}

	Data::DateTime dt;
	dt.SetCurrTime();
	GetNewName(buff, &dt);

	NEW_CLASS(fileStm, FileStream(buff, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(log, Text::UTF8Writer(fileStm));
	log->WriteSignature();
}

IO::FileLog::~FileLog()
{
	SDEL_TEXT(this->dateFormat);
	Text::StrDelNew(fileName);
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

	DEL_CLASS(mut);
	mut = 0;
}

void IO::FileLog::LogClosed()
{
	if (!closed)
	{
		mut->Lock();
		log->Close();
		mut->Unlock();
		closed = true;
	}
}
void IO::FileLog::LogAdded(Data::DateTime *time, const UTF8Char *logMsg, LogLevel logLev)
{
	mut->Lock();
	Bool newFile = false;
	UTF8Char buff[256];

	if (logStyle == ILogHandler::LOG_TYPE_PER_DAY)
	{
		if (time->GetDay() != lastVal)
		{
			newFile = true;
			GetNewName(buff, time);
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_MONTH)
	{
		if (time->GetMonth() != lastVal)
		{
			newFile = true;
			GetNewName(buff, time);
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_YEAR)
	{
		if (time->GetYear() != lastVal)
		{
			newFile = true;
			GetNewName(buff, time);
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_HOUR)
	{
		if (lastVal != (time->GetDay() * 24 + time->GetHour()))
		{
			newFile = true;
			GetNewName(buff, time);
		}
	}

	if (newFile)
	{
		log->Close();
		DEL_CLASS(log);
		DEL_CLASS(fileStm);

		NEW_CLASS(fileStm, IO::FileStream(buff, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		NEW_CLASS(log, Text::UTF8Writer(fileStm));
		log->WriteSignature();

		Text::StrConcat(time->ToString(buff, this->dateFormat), (const UTF8Char*)"Program running");
		log->WriteLine(buff);
		fileStm->Flush();
	}

	if (!this->closed)
	{
		time->ToString(buff, this->dateFormat);
		Text::StringBuilderUTF8 sb;
		sb.Append(buff);
		sb.Append(logMsg);
		log->WriteLine(sb.ToString());
	}
	mut->Unlock();
}
