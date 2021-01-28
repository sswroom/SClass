#include "Stdafx.h"
#include "IO/NullWriter.h"
#include "Text/MyString.h"

IO::NullWriter::NullWriter()
{
}

IO::NullWriter::~NullWriter()
{
}

Bool IO::NullWriter::Write(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::NullWriter::Write(const UTF8Char *str)
{
	return true;
}

Bool IO::NullWriter::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::NullWriter::WriteLine(const UTF8Char *str)
{
	return true;
}

Bool IO::NullWriter::WriteW(const WChar *str, UOSInt nChar)
{
	return true;
}

Bool IO::NullWriter::WriteW(const WChar *str)
{
	return true;
}

Bool IO::NullWriter::WriteLineW(const WChar *str, UOSInt nChar)
{
	return true;
}

Bool IO::NullWriter::WriteLineW(const WChar *str)
{
	return true;
}

Bool IO::NullWriter::WriteLine()
{
	return true;
}
