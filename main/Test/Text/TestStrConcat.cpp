#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Char sbuff[32];
	const Char *srcStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	Text::StrConcat(sbuff, srcStr);
	if (Text::StrEquals(sbuff, srcStr))
	{
		return 0;
	}
	return 1;
}
