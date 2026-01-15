#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/LogFileManager.h"
#include "IO/Path.h"
#include "Text/UTF8Reader.h"

#include <stdio.h>

IO::LogFileManager::LogFileManager(NN<Text::String> logPath)
{
	this->logPath = logPath->Clone();
}

IO::LogFileManager::~LogFileManager()
{
	this->logPath->Release();
}

void IO::LogFileManager::QueryLogMonths(NN<Data::ArrayListNative<UInt32>> months)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = this->logPath->ConcatTo(sbuff);
	sptr = Text::StrConcatC(sptr, UTF8STRC("??????"));
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sbuff, sess, 0, pt, 0).SetTo(sptr))
		{
			if (pt == IO::Path::PathType::Directory)
			{
				UInt32 month = Text::StrToUInt32(&sptr[-6]);
				if (month >= 200000)
				{
					months->Add(month);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

void IO::LogFileManager::QueryLogByMonth(NN<Data::ArrayListNative<UInt32>> dates, UInt32 month)
{
	if (month <= 200000 || month > 999912)
	{
		return;
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = this->logPath->ConcatTo(sbuff);
	sptr = Text::StrUInt32(sptr, month);
	UIntOS i = this->logPath->LastIndexOf(IO::Path::PATH_SEPERATOR);
	sptr = this->logPath->ToCString().Substring(i).ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("????????.log"));
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sbuff, sess, 0, pt, 0).SetTo(sptr))
		{
			if (pt == IO::Path::PathType::File)
			{
				sptr[-4] = 0;
				UInt32 date = Text::StrToUInt32(&sptr[-12]);
				if (date > 20000100 && date <= 99991231)
				{
					dates->Add(date);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

Optional<IO::Stream> IO::LogFileManager::OpenLogFile(UInt32 date)
{
	if (date <= 20000100 || date > 99991231)
	{
		return nullptr;
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = this->logPath->ConcatTo(sbuff);
	sptr = Text::StrUInt32(sptr, date / 100);
	UIntOS i = this->logPath->LastIndexOf(IO::Path::PATH_SEPERATOR);
	sptr = this->logPath->ToCString().Substring(i).ConcatTo(sptr);
	sptr = Text::StrUInt32(sptr, date);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".log"));
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return nullptr;
	}
	return fs;
}

void IO::LogFileManager::WriteLogText(NN<IO::Stream> fs, NN<Text::StyledTextWriter> writer)
{
	Text::UTF8Reader reader(fs);
	Text::StringBuilderUTF8 sb;
	while (reader.ReadLine(sb, 1024))
	{
		reader.GetLastLineBreak(sb);
		if (sb.leng <= 20)
		{
			writer->Write(sb.ToCString());
		}
		else if (sb.v[4] == '-' && sb.v[7] == '-' && sb.v[10] == ' ')
		{
			UIntOS i = sb.IndexOf('\t', 11);
			if (i != INVALID_INDEX)
			{
				writer->SetTextColor(Text::StandardColor::Blue);
				writer->Write(Text::CStringNN(sb.v, i));
				writer->ResetTextColor();
				writer->WriteChar(' ');
				writer->Write(sb.ToCString().Substring(i + 1));
			}
			else
			{
				writer->Write(sb.ToCString());
			}
		}
		else
		{
			writer->Write(sb.ToCString());
		}
		sb.ClearStr();
	}
}
