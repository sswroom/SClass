#include "Stdafx.h"
#include "IO/NullWriter.h"
#include "Text/MyString.h"

IO::NullWriter::NullWriter()
{
}

IO::NullWriter::~NullWriter()
{
}

Bool IO::NullWriter::Write(Text::CStringNN str)
{
	return true;
}

Bool IO::NullWriter::WriteLine(Text::CStringNN str)
{
	return true;
}

Bool IO::NullWriter::WriteLine()
{
	return true;
}
