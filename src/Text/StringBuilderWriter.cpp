#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/StringBuilderWriter.h"

Text::StringBuilderWriter::StringBuilderWriter(NN<Text::StringBuilderUTF8> sb)
{
	this->sb = sb;
}

Text::StringBuilderWriter::~StringBuilderWriter()
{
}

Bool Text::StringBuilderWriter::Write(Text::CStringNN str)
{
	this->sb->Append(str);
	return true;
}

Bool Text::StringBuilderWriter::WriteLine(Text::CStringNN str)
{
	this->sb->Append(str);
	this->sb->Append(CSTR("\r\n"));
	return true;
}

Bool Text::StringBuilderWriter::WriteLine()
{
	this->sb->AppendC(UTF8STRC("\r\n"));
	return true;
}
