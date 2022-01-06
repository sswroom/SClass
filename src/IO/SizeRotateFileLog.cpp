#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/FileUtil.h"
#include "IO/SizeRotateFileLog.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void IO::SizeRotateFileLog::SwapFiles()
{
	UTF8Char buff1[256];
	UTF8Char buff2[256];
	Int32 i;
	i = this->nFiles - 1;
	
	Text::StrConcatC(Text::StrInt32(Text::StrConcat(buff1, this->fileName), i), UTF8STRC(".log"));
	IO::FileUtil::DeleteFile(buff1, true);
	while (i-- > 0)
	{
		Text::StrConcatC(Text::StrInt32(Text::StrConcat(buff1, this->fileName), i), UTF8STRC(".log"));
		Text::StrConcatC(Text::StrInt32(Text::StrConcat(buff2, this->fileName), i + 1), UTF8STRC(".log"));
		IO::FileUtil::RenameFile(buff1, buff2);
	}
}

IO::SizeRotateFileLog::SizeRotateFileLog(const UTF8Char *fileName, Int32 nFiles, Int64 fileSize)
{
	NEW_CLASS(mut, Sync::Mutex());
	this->nFiles = nFiles;
	this->fileSize = fileSize;
	this->closed = false;

	UTF8Char buff[256];

	this->fileName = Text::StrCopyNew(fileName);
	this->extName = 0;

	NEW_CLASS(enc, Text::Encoding(65001));
	Text::StrConcatC(Text::StrConcat(buff, fileName), UTF8STRC("0.log"));
	NEW_CLASS(fileStm, IO::FileStream(buff, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(log, IO::StreamWriter(fileStm, enc));
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

IO::SizeRotateFileLog::~SizeRotateFileLog()
{
	Text::StrDelNew(fileName);
	fileName = 0;
	if (this->extName)
	{
		Text::StrDelNew(this->extName);
		this->extName = 0;
	}

	SDEL_CLASS(log);
	DEL_CLASS(enc);
	enc = 0;

	SDEL_CLASS(fileStm);
	fileStm = 0;

	DEL_CLASS(mut);
	mut = 0;
}

void IO::SizeRotateFileLog::LogClosed()
{
	if (!closed)
	{
		mut->Lock();
		log->Close();
		mut->Unlock();
		closed = true;
	}
}
void IO::SizeRotateFileLog::LogAdded(Data::DateTime *time, const UTF8Char *logMsg, LogLevel logLev)
{
	Bool newFile = false;
	UTF8Char buff[256];
	UTF8Char *sptr;

	mut->Lock();
	if (this->fileStm == 0 || this->fileStm->GetLength() > this->fileSize)
	{
		newFile = true;
	}

	if (newFile)
	{
		log->Close();
		DEL_CLASS(log);
		DEL_CLASS(fileStm);

		SwapFiles();
		Text::StrConcatC(Text::StrConcat(buff, fileName), UTF8STRC("0.log"));

		NEW_CLASS(fileStm, IO::FileStream(buff, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(log, IO::StreamWriter(fileStm, enc));
		log->WriteSignature();

		sptr = Text::StrConcatC(time->ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t"), UTF8STRC("Program running"));
		log->WriteLineC(buff, (UOSInt)(sptr - buff));
		fileStm->Flush();
	}

	if (!this->closed)
	{
		sptr = time->ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t");
		Text::StringBuilderUTF8 sb;
		sb.AppendC(buff, (UOSInt)(sptr - buff));
		sb.Append(logMsg);
		log->WriteLineC(sb.ToString(), sb.GetLength());
	}
	mut->Unlock();
}
