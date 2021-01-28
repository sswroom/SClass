#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/StringBuilderWriter.h"

Text::StringBuilderWriter::StringBuilderWriter(Text::StringBuilderUTF *sb)
{
	this->sb = sb;
}

Text::StringBuilderWriter::~StringBuilderWriter()
{
}

Bool Text::StringBuilderWriter::Write(const UTF8Char *str, UOSInt nChar)
{
	this->sb->AppendC(str, nChar);
	return true;
}

Bool Text::StringBuilderWriter::Write(const UTF8Char *str)
{
	this->sb->Append(str);
	return true;
}

Bool Text::StringBuilderWriter::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	this->sb->AppendC(str, nChar);
	this->sb->AppendC((const UTF8Char*)"\r\n", 2);
	return true;
}

Bool Text::StringBuilderWriter::WriteLine(const UTF8Char *str)
{
	this->sb->Append(str);
	this->sb->AppendC((const UTF8Char*)"\r\n", 2);
	return true;
}

Bool Text::StringBuilderWriter::WriteLine()
{
	this->sb->AppendC((const UTF8Char*)"\r\n", 2);
	return true;
}
