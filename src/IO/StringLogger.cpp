#include "Stdafx.h"
#include "IO/StringLogger.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::StringLogger::StringLogger()
{
	this->modified = false;
}

IO::StringLogger::~StringLogger()
{
	LIST_FREE_STRING(&this->strList);
}

Bool IO::StringLogger::IsModified()
{
	return this->modified;
}

void IO::StringLogger::ReadLogs(IO::Reader *reader)
{
	Text::StringBuilderUTF8 sb;
	while (reader->ReadLine(&sb, 4096))
	{
		this->LogStr(sb.ToString(), sb.GetLength());
		sb.ClearStr();
	}
	this->modified = false;
}

void IO::StringLogger::LogStr(const UTF8Char *s, UOSInt len)
{
	Sync::MutexUsage mutUsage(&this->mut);
	OSInt i = this->strList.SortedIndexOfPtr(s, len);
	if (i < 0)
	{
		this->strList.Insert((UOSInt)~i, Text::String::New(s, len).Ptr());
		this->modified = true;
	}
}

void IO::StringLogger::WriteLogs(IO::Writer *writer)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Text::String *s;
	UOSInt i = 0;
	UOSInt j = this->strList.GetCount();	
	this->modified = false;
	while (i < j)
	{
		s = this->strList.GetItem(i);
		writer->WriteLineC(s->v, s->leng);
		i++;
	}
}
