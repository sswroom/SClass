#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Char sbuff[32];
	Char sbuff2[32];
	const Char *srcStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	Text::StrConcatC(sbuff, srcStr, 26);
	Text::StrConcatC(sbuff2, srcStr, 25);
	if (Text::StrEquals(sbuff, srcStr) && !Text::StrEquals(sbuff2, srcStr))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
