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

UnsafeArray<const WChar> Text::StringUtil::ToString(NN<Data::DateTime> dt, UnsafeArray<const Char> pattern)
{
	Char tmpbuff[32];
	dt->ToString(tmpbuff, pattern);
	Text::StrConcatASCII(wbuff, tmpbuff);
	return wbuff;
}

UnsafeArray<const WChar> Text::StringUtil::ToString(Double val)
{
	Text::StrDoubleW(wbuff, val);
	return wbuff;
}

UnsafeArray<const WChar> Text::StringUtil::ToString(Double val, UnsafeArray<const Char> pattern)
{
	Text::StrDoubleFmtW(wbuff, val, pattern);
	return wbuff;
}

UnsafeArray<const WChar> Text::StringUtil::ToString(Int32 val)
{
	Text::StrInt32(wbuff, val);
	return wbuff;
}

UnsafeArray<const WChar> Text::StringUtil::ToString(Int64 val)
{
	Text::StrInt64(wbuff, val);
	return wbuff;
}
