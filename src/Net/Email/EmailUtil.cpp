#include "Stdafx.h"
#include "Net/Email/EmailUtil.h"
#include "Text/MyString.h"

Bool Net::Email::EmailUtil::AddressValid(const UTF8Char *addr)
{
	UOSInt i = Text::StrIndexOf(addr, '@');
	if (i == INVALID_INDEX || i == 0)
	{
		return false;
	}
	i = Text::StrIndexOf(&addr[i + 1], '@');
	if (i != INVALID_INDEX)
	{
		return false;
	}
	return true;
}
