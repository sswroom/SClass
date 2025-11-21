#include "Stdafx.h"
#include "IO/StringLogFile.h"

IO::StringLogFile::StringLogFile(NN<Text::String> sourceName) : IO::LogFile(sourceName)
{
}

IO::StringLogFile::~StringLogFile()
{
	NN<LogItem> item;
	UOSInt i = this->items.GetCount();
	while (i-- > 0)
	{
		item = this->items.GetItemNoCheck(i);
		OPTSTR_DEL(item->message);
		OPTSTR_DEL(item->desc);
		MemFreeNN(item);
	}
}

UOSInt IO::StringLogFile::GetCount(IO::LogHandler::LogLevel logLevel) const
{
	return this->items.GetCount();
}

Bool IO::StringLogFile::GetLogMessage(IO::LogHandler::LogLevel logLevel, UOSInt index, OutParam<Data::Timestamp> ts, NN<Text::StringBuilderUTF8> sb, Text::LineBreakType lineBreak) const
{
	NN<LogItem> item;
	if (!this->items.GetItem(index).SetTo(item))
	{
		return false;
	}
	ts.Set(item->ts);
	NN<Text::String> s;
	if (item->message.SetTo(s))
	{
		sb->Append(s);
	}
	return true;
}

Bool IO::StringLogFile::GetLogDescription(IO::LogHandler::LogLevel logLevel, UOSInt index, NN<Text::StringBuilderUTF8> sb) const
{
	NN<LogItem> item;
	if (!this->items.GetItem(index).SetTo(item))
	{
		return false;
	}
	NN<Text::String> s;
	if (item->desc.SetTo(s))
	{
		sb->Append(s);
	}
	return true;
}

UOSInt IO::StringLogFile::AddLog(const Data::Timestamp &ts, Text::CString message, Text::CString desc)
{
	NN<LogItem> item = MemAllocNN(LogItem);
	item->ts = ts;
	item->message = Text::String::NewOrNull(message);
	item->desc = Text::String::NewOrNull(desc);
	return this->items.Add(item);
}
