#include "Stdafx.h"
#include "IO/StringLogFile.h"

IO::StringLogFile::StringLogFile(NotNullPtr<Text::String> sourceName) : IO::LogFile(sourceName)
{
}

IO::StringLogFile::~StringLogFile()
{
	LogItem *item;
	UOSInt i = this->items.GetCount();
	while (i-- > 0)
	{
		item = this->items.GetItem(i);
		OPTSTR_DEL(item->message);
		OPTSTR_DEL(item->desc);
		MemFree(item);
	}
}

UOSInt IO::StringLogFile::GetCount(IO::LogHandler::LogLevel logLevel) const
{
	return this->items.GetCount();
}

Bool IO::StringLogFile::GetLogMessage(IO::LogHandler::LogLevel logLevel, UOSInt index, Data::Timestamp *ts, NotNullPtr<Text::StringBuilderUTF8> sb, Text::LineBreakType lineBreak) const
{
	LogItem *item = this->items.GetItem(index);
	if (item == 0)
	{
		return false;
	}
	*ts = item->ts;
	NotNullPtr<Text::String> s;
	if (item->message.SetTo(s))
	{
		sb->Append(s);
	}
	return true;
}

Bool IO::StringLogFile::GetLogDescription(IO::LogHandler::LogLevel logLevel, UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	LogItem *item = this->items.GetItem(index);
	if (item == 0)
	{
		return false;
	}
	NotNullPtr<Text::String> s;
	if (item->desc.SetTo(s))
	{
		sb->Append(s);
	}
	return true;
}

UOSInt IO::StringLogFile::AddLog(const Data::Timestamp &ts, Text::CString message, Text::CString desc)
{
	LogItem *item = MemAlloc(LogItem, 1);
	item->ts = ts;
	item->message = Text::String::NewOrNull(message);
	item->desc = Text::String::NewOrNull(desc);
	return this->items.Add(item);
}
