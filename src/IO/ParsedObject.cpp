#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/ParsedObject.h"

IO::ParsedObject::ParsedObject(const UTF8Char *sourceName)
{
	if (sourceName)
	{
		this->sourceName = Text::StrCopyNew(sourceName);
	}
	else
	{
		this->sourceName = 0;
	}
}

IO::ParsedObject::~ParsedObject()
{
	if (this->sourceName)
	{
		Text::StrDelNew(this->sourceName);
		this->sourceName = 0;
	}
}

UTF8Char *IO::ParsedObject::GetSourceName(UTF8Char *oriStr)
{
	if (this->sourceName)
		return Text::StrConcat(oriStr, this->sourceName);
	return 0;
}

const UTF8Char *IO::ParsedObject::GetSourceNameObj()
{
	return this->sourceName;
}

void IO::ParsedObject::SetSourceName(const UTF8Char *sourceName)
{
	if (this->sourceName)
		Text::StrDelNew(this->sourceName);
	if (sourceName)
	{
		this->sourceName = Text::StrCopyNew(sourceName);
	}
	else
	{
		this->sourceName = 0;
	}

}
