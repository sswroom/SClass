#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringUtil.h"

Text::StringUtil::StringUtil()
{
}

Text::StringUtil::~StringUtil()
{
}

const WChar *Text::StringUtil::ToString(Data::DateTime *dt, const Char *pattern)
{
	Char tmpbuff[32];
	dt->ToString(tmpbuff, pattern);
	Text::StrConcatASCII(sbuff, tmpbuff);
	return sbuff;
}

const WChar *Text::StringUtil::ToString(Double val)
{
	Text::StrDouble(sbuff, val);
	return sbuff;
}

const WChar *Text::StringUtil::ToString(Double val, const Char *pattern)
{
	Text::StrDoubleFmt(sbuff, val, pattern);
	return sbuff;
}

const WChar *Text::StringUtil::ToString(Int32 val)
{
	Text::StrInt32(sbuff, val);
	return sbuff;
}

const WChar *Text::StringUtil::ToString(Int64 val)
{
	Text::StrInt64(sbuff, val);
	return sbuff;
}
