#include "Stdafx.h"
#include "IO/StringLogFile.h"

IO::StringLogFile::StringLogFile(Text::String *sourceName) : IO::LogFile(sourceName)
{
}

IO::StringLogFile::~StringLogFile()
{
	LogItem *item;
	UOSInt i = this->items.GetCount();
	while (i-- > 0)
	{
		item = this->items.GetItem(i);
		SDEL_STRING(item->message);
		SDEL_STRING(item->desc);
		MemFree(item);
	}
}

UOSInt IO::StringLogFile::GetCount(IO::ILogHandler::LogLevel logLevel) const
{
	return this->items.GetCount();
}

Bool IO::StringLogFile::GetLogMessage(IO::ILogHandler::LogLevel logLevel, UOSInt index, Data::Timestamp *ts, Text::StringBuilderUTF8 *sb, Text::LineBreakType lineBreak) const
{
	LogItem *item = this->items.GetItem(index);
	if (item == 0)
	{
		return false;
	}
	*ts = item->ts;
	if (item->message)
	{
		sb->Append(item->message);
	}
	return true;
}

Bool IO::StringLogFile::GetLogDescription(IO::ILogHandler::LogLevel logLevel, UOSInt index, Text::StringBuilderUTF8 *sb) const
{
	LogItem *item = this->items.GetItem(index);
	if (item == 0)
	{
		return false;
	}
	if (item->desc)
	{
		sb->Append(item->desc);
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
