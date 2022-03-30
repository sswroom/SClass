#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WChar wbuff[32];
	Int64 i;
	Int64 j;
//	i = 100000;
	i = 10000;
	while (i-- > 0)
	{
		Text::StrInt64(wbuff, i);
		j = Text::StrToInt64(wbuff);
		if (i != j)
		{
			printf("%lld != %lld\r\n", i, j);
			return 1;
		}
	}
	i = 10000100000LL;
	while (i-- > 10000000000LL)
	{
		Text::StrInt64(wbuff, i);
		j = Text::StrToInt64(wbuff);
		if (i != j)
		{
			printf("%lld != %lld\r\n", i, j);
			return 1;
		}
	}
//	i = 0x100010000LL;
//	while (i-- > 0xffff0000LL)
	i = 0x10000010LL;
	while (i-- > 0xffffFFF0LL)
	{
		Text::StrInt64(wbuff, i);
		j = Text::StrToInt64(wbuff);
		if (i != j)
		{
			printf("%lld != %lld\r\n", i, j);
			return 1;
		}
	}
	i = -100000;
	while (i++ < 0)
	{
		Text::StrInt64(wbuff, i);
		j = Text::StrToInt64(wbuff);
		if (i != j)
		{
			printf("%lld != %lld\r\n", i, j);
			return 1;
		}
	}
	i = -10000100000LL;
	while (i++ < -10000000000LL)
	{
		Text::StrInt64(wbuff, i);
		j = Text::StrToInt64(wbuff);
		if (i != j)
		{
			printf("%lld != %lld\r\n", i, j);
			return 1;
		}
	}
	return 0;
}
