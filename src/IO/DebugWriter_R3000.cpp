#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DebugWriter.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include <syslog.h>

IO::DebugWriter::DebugWriter()
{
	NEW_CLASS(this->clsData, Text::StringBuilderUTF8());
}

IO::DebugWriter::~DebugWriter()
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	DEL_CLASS(sb);
}

void IO::DebugWriter::Flush()
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	OSInt i;
	UTF8Char *sarr[2];
	i = Text::StrSplitLine(sarr, 2, sb->ToString());
	if (i == 2)
	{
		while (i == 2)
		{
			syslog(LOG_DEBUG, (const Char*)sarr[0]);
			i = Text::StrSplitLine(sarr, 2, sarr[1]);
		}
		sb->SetSubstr(sb->GetLength() - Text::StrCharCnt(sarr[0]));
	}
}

Bool IO::DebugWriter::Write(const UTF8Char *str, UOSInt nChar)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->Append(str, nChar);
	this->Flush();
	return true;
}

Bool IO::DebugWriter::Write(const UTF8Char *str)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->Append(str);
	this->Flush();
	return true;
}

Bool IO::DebugWriter::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->Append(str, nChar);
	this->Flush();
	syslog(LOG_DEBUG, (const Char*)sb->ToString());
	sb->ClearStr();
	return true;
}

Bool IO::DebugWriter::WriteLine(const UTF8Char *str)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->Append(str);
	this->Flush();
	syslog(LOG_DEBUG, (const Char*)sb->ToString());
	sb->ClearStr();
	return true;
}

Bool IO::DebugWriter::WriteW(const WChar *str, UOSInt nChar)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->AppendW(str, nChar);
	this->Flush();
	return true;
}

Bool IO::DebugWriter::WriteW(const WChar *str)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->AppendW(str);
	this->Flush();
	return true;
}

Bool IO::DebugWriter::WriteLineW(const WChar *str, UOSInt nChar)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->AppendW(str, nChar);
	this->Flush();
	syslog(LOG_DEBUG, (const Char*)sb->ToString());
	sb->ClearStr();
	return true;
}

Bool IO::DebugWriter::WriteLineW(const WChar *str)
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	sb->AppendW(str);
	this->Flush();
	syslog(LOG_DEBUG, (const Char*)sb->ToString());
	sb->ClearStr();
	return true;
}

Bool IO::DebugWriter::WriteLine()
{
	Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->clsData;
	syslog(LOG_DEBUG, (const Char*)sb->ToString());
	sb->ClearStr();
	return true;
}
