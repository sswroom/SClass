#include "Stdafx.h"
#include "IO/NullWriter.h"
#include "Text/MyString.h"

IO::NullWriter::NullWriter()
{
}

IO::NullWriter::~NullWriter()
{
}

Bool IO::NullWriter::WriteStrC(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::NullWriter::WriteStr(const UTF8Char *str)
{
	return true;
}

Bool IO::NullWriter::WriteLineC(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::NullWriter::WriteLine(const UTF8Char *str)
{
	return true;
}

Bool IO::NullWriter::WriteLine()
{
	return true;
}
