#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	WChar wbuff[32];
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	Text::StrConcat(wbuff, srcStr);
	if (Text::StrEquals(wbuff, srcStr))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
