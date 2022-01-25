#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/StringBuilderWriter.h"

Text::StringBuilderWriter::StringBuilderWriter(Text::StringBuilderUTF8 *sb)
{
	this->sb = sb;
}

Text::StringBuilderWriter::~StringBuilderWriter()
{
}

Bool Text::StringBuilderWriter::WriteStrC(const UTF8Char *str, UOSInt nChar)
{
	this->sb->AppendC(str, nChar);
	return true;
}

Bool Text::StringBuilderWriter::WriteStr(const UTF8Char *str)
{
	this->sb->Append(str);
	return true;
}

Bool Text::StringBuilderWriter::WriteLineC(const UTF8Char *str, UOSInt nChar)
{
	this->sb->AppendC(str, nChar);
	this->sb->AppendC(UTF8STRC("\r\n"));
	return true;
}

Bool Text::StringBuilderWriter::WriteLine(const UTF8Char *str)
{
	this->sb->Append(str);
	this->sb->AppendC(UTF8STRC("\r\n"));
	return true;
}

Bool Text::StringBuilderWriter::WriteLine()
{
	this->sb->AppendC(UTF8STRC("\r\n"));
	return true;
}
