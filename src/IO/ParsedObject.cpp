#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/ParsedObject.h"

IO::ParsedObject::ParsedObject(Text::String *sourceName)
{
	this->sourceName = SCOPY_STRING(sourceName);
}

IO::ParsedObject::ParsedObject(Text::CString sourceName)
{
	this->sourceName = Text::String::NewOrNull(sourceName);
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

void IO::ParsedObject::SetSourceName(Text::CString sourceName)
{
	SDEL_STRING(this->sourceName);
	this->sourceName = Text::String::NewOrNull(sourceName);
}
