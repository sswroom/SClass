#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/ParsedObject.h"

IO::ParsedObject::ParsedObject(Text::String *sourceName)
{
	this->sourceName = SCOPY_STRING(sourceName);
}

IO::ParsedObject::ParsedObject(const UTF8Char *sourceName)
{
	this->sourceName = Text::String::NewOrNullSlow(sourceName);
}

IO::ParsedObject::ParsedObject(Text::CString sourceName)
{
	if (sourceName.v)
	{
		this->sourceName = Text::String::New(sourceName.v, sourceName.leng);
	}
	else
	{
		this->sourceName = 0;
	}
}

IO::ParsedObject::~ParsedObject()
{
	SDEL_STRING(this->sourceName);
}

UTF8Char *IO::ParsedObject::GetSourceName(UTF8Char *oriStr)
{
	if (this->sourceName)
		return this->sourceName->ConcatTo(oriStr);
	return 0;
}

Text::String *IO::ParsedObject::GetSourceNameObj()
{
	return this->sourceName;
}

void IO::ParsedObject::SetSourceName(Text::String *sourceName)
{
	SDEL_STRING(this->sourceName);
	this->sourceName = SCOPY_STRING(sourceName);
}

void IO::ParsedObject::SetSourceName(const UTF8Char *sourceName, UOSInt nameLen)
{
	SDEL_STRING(this->sourceName);
	if (sourceName)
	{
		this->sourceName = Text::String::New(sourceName, nameLen);
	}
}
