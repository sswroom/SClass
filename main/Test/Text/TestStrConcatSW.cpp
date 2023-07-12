#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	WChar wbuff[32];
	WChar wbuff2[32];
	WChar wbuff3[32];
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	Text::StrConcatS(wbuff, srcStr, 26);
	Text::StrConcatS(wbuff2, srcStr, 25);
	Text::StrConcatS(wbuff3, srcStr, 27);
	if (Text::StrEquals(wbuff, srcStr) && !Text::StrEquals(wbuff2, srcStr) && Text::StrEquals(wbuff3, srcStr))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
