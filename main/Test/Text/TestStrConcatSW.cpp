#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WChar sbuff[32];
	WChar sbuff2[32];
	WChar sbuff3[32];
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	Text::StrConcatS(sbuff, srcStr, 26);
	Text::StrConcatS(sbuff2, srcStr, 25);
	Text::StrConcatS(sbuff3, srcStr, 27);
	if (Text::StrEquals(sbuff, srcStr) && !Text::StrEquals(sbuff2, srcStr) && Text::StrEquals(sbuff3, srcStr))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
