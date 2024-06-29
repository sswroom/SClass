#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	WChar wbuff[32];
	Int32 i = 1000000;
	Int32 j;
	while (i-- > -10000)
	{
		Text::StrInt32(wbuff, i);
		j = Text::StrToInt32W(UnsafeArray<WChar>(wbuff));
		if (i != j)
		{
			return 1;
		}
	}
	return 0;
}
