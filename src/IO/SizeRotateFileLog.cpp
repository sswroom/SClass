#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/FileUtil.h"
#include "IO/SizeRotateFileLog.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void IO::SizeRotateFileLog::SwapFiles()
{
	UTF8Char buff1[256];
	UTF8Char buff2[256];
	UnsafeArray<UTF8Char> sptr1;
	UIntOS i;
	i = this->nFiles - 1;
	
	sptr1 = Text::StrConcatC(Text::StrUIntOS(this->fileName->ConcatTo(buff1), i), UTF8STRC(".log"));
	IO::FileUtil::DeleteFile(CSTRP(buff1, sptr1), true);
	while (i-- > 0)
	{
		Text::StrConcatC(Text::StrUIntOS(this->fileName->ConcatTo(buff1), i), UTF8STRC(".log"));
		Text::StrConcatC(Text::StrUIntOS(this->fileName->ConcatTo(buff2), i + 1), UTF8STRC(".log"));
		IO::FileUtil::RenameFile(buff1, buff2);
	}
}

IO::SizeRotateFileLog::SizeRotateFileLog(Text::CStringNN fileName, UIntOS nFiles, UInt64 fileSize)
{
	this->nFiles = nFiles;
	this->fileSize = fileSize;
	this->closed = false;

	UTF8Char buff[256];
	UnsafeArray<UTF8Char> sptr;

	this->fileName = Text::String::New(fileName);
	this->extName = 0;

	sptr = Text::StrConcatC(fileName.ConcatTo(buff), UTF8STRC("0.log"));
	NEW_CLASSNN(this->fileStm, IO::FileStream(CSTRP(buff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASSNN(this->log, Text::UTF8Writer(fileStm));
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

IO::SizeRotateFileLog::~SizeRotateFileLog()
{
	fileName->Release();
	if (this->extName)
	{
		Text::StrDelNew(this->extName);
		this->extName = 0;
	}

	log.Delete();
	fileStm.Delete();
}

void IO::SizeRotateFileLog::LogClosed()
{
	if (!closed)
	{
		Sync::MutexUsage(this->mut);
		log->Close();
		closed = true;
	}
}
void IO::SizeRotateFileLog::LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev)
{
	Bool newFile = false;
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> sptr;

	Sync::MutexUsage mutUsage(this->mut);
	if (this->fileStm->GetLength() > this->fileSize)
	{
		newFile = true;
	}

	if (newFile)
	{
		log->Close();
		log.Delete();
		fileStm.Delete();

		SwapFiles();
		sptr = Text::StrConcatC(fileName->ConcatTo(buff), UTF8STRC("0.log"));

		NEW_CLASSNN(this->fileStm, IO::FileStream(CSTRP(buff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASSNN(this->log, Text::UTF8Writer(this->fileStm));
		log->WriteSignature();

		sptr = Text::StrConcatC(logTime.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t"), UTF8STRC("Program running"));
		log->WriteLine(CSTRP(buff, sptr));
		fileStm->Flush();
	}

	if (!this->closed)
	{
		sptr = logTime.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff\t");
		Text::StringBuilderUTF8 sb;
		sb.AppendC(buff, (UIntOS)(sptr - buff));
		sb.Append(logMsg);
		log->WriteLine(sb.ToCString());
	}
}
