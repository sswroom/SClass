#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Text/MyString.h"
#include "IO/FileUtil.h"
#include "IO/SizeRotateFileLog.h"
#include "Text/StringBuilder.h"

void IO::SizeRotateFileLog::SwapFiles()
{
	WChar buff1[256];
	WChar buff2[256];
	Int32 i;
	i = this->nFiles - 1;
	
	Text::StrConcat(Text::StrInt32(Text::StrConcat(buff1, this->fileName), i), L".log");
	IO::FileUtil::DeleteFile(buff1, true);
	while (i-- > 0)
	{
		Text::StrConcat(Text::StrInt32(Text::StrConcat(buff1, this->fileName), i), L".log");
		Text::StrConcat(Text::StrInt32(Text::StrConcat(buff2, this->fileName), i + 1), L".log");
		IO::FileUtil::RenameFile(buff1, buff2);
	}
}

IO::SizeRotateFileLog::SizeRotateFileLog(const WChar *fileName, Int32 nFiles, Int64 fileSize)
{
	NEW_CLASS(mut, Sync::Mutex());
	this->nFiles = nFiles;
	this->fileSize = fileSize;
	this->closed = false;

	WChar buff[256];

	this->fileName = Text::StrCopyNew(fileName);
	this->extName = 0;

	NEW_CLASS(enc, Text::Encoding(65001));
	Text::StrConcat(Text::StrConcat(buff, fileName), L"0.log");
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
void IO::SizeRotateFileLog::LogAdded(Data::DateTime *time, const WChar *logMsg, LogLevel logLev)
{
	Bool newFile = false;
	WChar buff[256];

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
		Text::StrConcat(Text::StrConcat(buff, fileName), L"0.log");

		NEW_CLASS(fileStm, IO::FileStream(buff, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(log, IO::StreamWriter(fileStm, enc));
		log->WriteSignature();

		Text::StrConcat(time->ToString(buff, L"yyyy-MM-dd HH:mm:ss.fff\t"), L"Program running");
		log->WriteLine(buff);
		fileStm->Flush();
	}

	if (!this->closed)
	{
		time->ToString(buff, L"yyyy-MM-dd HH:mm:ss.fff\t");
		Text::StringBuilder sb;
		sb.Append(buff);
		sb.Append(logMsg);
		log->WriteLineC(sb.ToString(), sb.GetLength());
	}
	mut->Unlock();
}
