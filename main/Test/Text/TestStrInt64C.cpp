#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Char sbuff[32];
	Int64 i;
	Int64 j;
	i = 100000;
	while (i-- > 0)
	{
		Text::StrInt64(sbuff, i);
		j = Text::StrToInt64(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	i = 10000100000LL;
	while (i-- > 10000000000LL)
	{
		Text::StrInt64(sbuff, i);
		j = Text::StrToInt64(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	i = 0x100010000LL;
	while (i-- > 0xffff0000LL)
	{
		Text::StrInt64(sbuff, i);
		j = Text::StrToInt64(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	i = -100000;
	while (i++ < 0)
	{
		Text::StrInt64(sbuff, i);
		j = Text::StrToInt64(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	i = -10000100000LL;
	while (i++ < -10000000000LL)
	{
		Text::StrInt64(sbuff, i);
		j = Text::StrToInt64(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	return 0;
}
