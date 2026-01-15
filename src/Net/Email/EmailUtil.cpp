#include "Stdafx.h"
#include "Net/Email/EmailUtil.h"
#include "Text/MyString.h"

Bool Net::Email::EmailUtil::AddressValid(UnsafeArray<const UTF8Char> addr)
{
	UIntOS i = Text::StrIndexOfChar(addr, '@');
	if (i == INVALID_INDEX || i == 0)
	{
		return false;
	}
	i = Text::StrIndexOfChar(&addr[i + 1], '@');
	if (i != INVALID_INDEX)
	{
		return false;
	}
	return true;
}
