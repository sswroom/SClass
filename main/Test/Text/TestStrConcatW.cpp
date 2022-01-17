#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WChar sbuff[32];
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	Text::StrConcat(sbuff, srcStr);
	if (Text::StrEquals(sbuff, srcStr))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
