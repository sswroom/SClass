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
	Text::StrConcatC(Text::StrInt32(Text::StrConcat(buff1, this->fileName), i), UTF8STRC(".log"));
	Text::StrConcatC(Text::StrConcat(buff2, this->fileName), UTF8STRC("0.tmp"));
	IO::FileUtil::RenameFile(buff1, buff2);
	while (i-- > 0)
	{
		Text::StrConcatC(Text::StrInt32(Text::StrConcat(buff1, this->fileName), i), UTF8STRC(".log"));
		Text::StrConcatC(Text::StrInt32(Text::StrConcat(buff2, this->fileName), i + 1), UTF8STRC(".log"));
		IO::FileUtil::RenameFile(buff1, buff2);
	}
	Text::StrConcatC(Text::StrConcat(buff1, this->fileName), UTF8STRC("0.tmp"));
	Text::StrConcatC(Text::StrConcat(buff2, this->fileName), UTF8STRC("0.log"));
	IO::FileUtil::RenameFile(buff1, buff2);
}

IO::LoopFileLog::LoopFileLog(const UTF8Char *fileName, Int32 nFiles, LogType style)
{
	NEW_CLASS(mut, Sync::Mutex());
	this->logStyle = style;
	this->nFiles = nFiles;
	this->closed = false;

	UTF8Char buff[256];
	UTF8Char *sptr;

	this->fileName = Text::StrCopyNew(fileName);
	SwapFiles();

	sptr = Text::StrConcatC(Text::StrConcat(buff, fileName), UTF8STRC("0.log"));
	NEW_CLASS(fileStm, IO::FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(log, Text::UTF8Writer(fileStm));
	log->WriteSignature();

	Data::DateTime dt;
	dt.SetCurrTime();
	if (logStyle == ILogHandler::LOG_TYPE_PER_DAY)
	{
		lastVal = dt.GetDay();
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_MONTH)
	{
		lastVal = dt.GetMonth();
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_YEAR)
	{
		lastVal = dt.GetYear();
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_HOUR)
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

void IO::LoopFileLog::LogClosed()
{
	if (!closed)
	{
		Sync::MutexUsage mutUsage(mut);
		log->Close();
		mutUsage.EndUse();
		closed = true;
	}
}
void IO::LoopFileLog::LogAdded(Data::DateTime *time, const UTF8Char *logMsg, UOSInt msgLen, LogLevel logLev)
{
	Bool newFile = false;
	UTF8Char buff[256];
	UTF8Char *sptr;

	Sync::MutexUsage mutUsage(mut);

	if (logStyle == ILogHandler::LOG_TYPE_PER_DAY)
	{
		if (time->GetDay() != lastVal)
		{
			lastVal = time->GetDay();
			newFile = true;
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_MONTH)
	{
		if (time->GetMonth() != lastVal)
		{
			lastVal = time->GetMonth();
			newFile = true;
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_YEAR)
	{
		if (time->GetYear() != lastVal)
		{
			lastVal = time->GetYear();
			newFile = true;
		}
	}
	else if (logStyle == ILogHandler::LOG_TYPE_PER_HOUR)
	{
		if (lastVal != (time->GetDay() * 24 + time->GetHour()))
		{
			lastVal = time->GetDay() * 24 + time->GetHour();
			newFile = true;
		}
	}

	if (newFile)
	{
		log->Close();
		DEL_CLASS(log);
		DEL_CLASS(fileStm);

		SwapFiles();
		sptr = Text::StrConcatC(Text::StrConcat(buff, fileName), UTF8STRC("0.log"));

		NEW_CLASS(fileStm, IO::FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(log, Text::UTF8Writer(fileStm));
		log->WriteSignature();

		sptr = Text::StrConcatC(time->ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t"), UTF8STRC("Program running"));
		log->WriteLineC(buff, (UOSInt)(sptr - buff));
		fileStm->Flush();
	}

	if (!this->closed)
	{
		sptr = time->ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t");
		Text::StringBuilderUTF8 sb;
		sb.AppendP(buff, sptr);
		sb.AppendC(logMsg, msgLen);
		log->WriteLineC(sb.ToString(), sb.GetLength());
	}
	mutUsage.EndUse();
}
