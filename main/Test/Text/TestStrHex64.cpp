#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[32];
	UInt64 i;
	UInt64 j;
	i = 100000;
	while (i-- > 0)
	{
		Text::StrHexVal64(sbuff, i);
		j = (UInt64)Text::StrHex2Int64C(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	i = 10000100000LL;
	while (i-- > 10000000000LL)
	{
		Text::StrHexVal64(sbuff, i);
		j = (UInt64)Text::StrHex2Int64C(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	i = (UInt64)-100000;
	while (i++ != 0)
	{
		Text::StrHexVal64(sbuff, i);
		j = (UInt64)Text::StrHex2Int64C(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	i = (UInt64)-10000100000LL;
	while (i++ != (UInt64)-10000000000LL)
	{
		Text::StrHexVal64(sbuff, i);
		j = (UInt64)Text::StrHex2Int64C(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	return 0;
}
