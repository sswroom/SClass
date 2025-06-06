#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	if (Text::StrCompareICaseCh("aBcDe1FgHiJk", "AbCdE1fGhIjK") != 0)
	{
		return 1;
	}
	if (Text::StrCompareICase((const UTF8Char*)"aBcDe1FgHiJk", (const UTF8Char*)"AbCdE1fGhIjK") != 0)
	{
		return 1;
	}
	if (Text::StrCompareICase(L"aBcDe1FgHiJk", L"AbCdE1fGhIjK") != 0)
	{
		return 1;
	}
	return 0;
}
