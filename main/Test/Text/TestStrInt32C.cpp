#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Char sbuff[32];
	Int32 i = 1000000;
	Int32 j;
	while (i-- > -10000)
	{
		Text::StrInt32(sbuff, i);
		j = Text::StrToInt32(sbuff);
		if (i != j)
		{
			return 1;
		}
	}
	return 0;
}
