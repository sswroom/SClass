#include "Stdafx.h"
#include "IO/StringLogger.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::StringLogger::StringLogger()
{
	this->modified = false;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->strList, Data::ArrayListStrUTF8());
}

IO::StringLogger::~StringLogger()
{
	DEL_LIST_FUNC(this->strList, Text::StrDelNew);
	DEL_CLASS(this->strList);
	DEL_CLASS(this->mut);
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
		this->LogStr(sb.ToString());
		sb.ClearStr();
	}
	this->modified = false;
}

void IO::StringLogger::LogStr(const UTF8Char *s)
{
	Sync::MutexUsage mutUsage(this->mut);
	OSInt i = this->strList->SortedIndexOf(s);
	if (i < 0)
	{
		this->strList->Insert(~i, Text::StrCopyNew(s));
		this->modified = true;
	}
}

void IO::StringLogger::WriteLogs(IO::Writer *writer)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt i = 0;
	UOSInt j = this->strList->GetCount();	
	this->modified = false;
	while (i < j)
	{
		writer->WriteLine(this->strList->GetItem(i));
		i++;
	}
}
