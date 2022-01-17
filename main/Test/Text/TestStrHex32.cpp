#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[32];
	UInt32 i = 100000;
	UInt32 j;
	while (i-- > 0)
	{
		Text::StrHexVal32V(sbuff, i);
		j = (UInt32)Text::StrHex2Int32C(sbuff);
		if (i != j)
		{
			return 1;
		}
	}

	return 0;
}
