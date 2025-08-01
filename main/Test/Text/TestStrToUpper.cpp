#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrToUpper(sbuff, (const UTF8Char*)"aBcDe1FgHiJk");
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ABCDE1FGHIJK")))
	{
		return 1;
	}
	sptr = Text::StrToLower(sbuff, (const UTF8Char*)"aBcDe1FgHiJk");
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("abcde1fghijk")))
	{
		return 1;
	}
	sptr = Text::StrToUpperC(sbuff, UTF8STRC("aBcDe1FgHiJk"));
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ABCDE1FGHIJK")))
	{
		return 1;
	}
	sptr = Text::StrToLowerC(sbuff, UTF8STRC("aBcDe1FgHiJk"));
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("abcde1fghijk")))
	{
		return 1;
	}
	if (!Text::StrEqualsICase((const UTF8Char*)"aBcDe1FgHiJk", (const UTF8Char*)"AbCdE1fGhIjK"))
	{
		return 1;
	}
	if (!Text::StrEqualsICaseC(UTF8STRC("aBcDe1FgHiJk"), UTF8STRC("AbCdE1fGhIjK")))
	{
		return 1;
	}
	if (Text::StrEqualsICaseC(UTF8STRC("aBcDe1FgHiJk"), UTF8STRC("AbCdE1fGhIjKa")))
	{
		return 1;
	}
	if (Text::StrEqualsICaseC(UTF8STRC("aBcDe1FgHiJka"), UTF8STRC("AbCdE1fGhIjK")))
	{
		return 1;
	}
	if (Text::StrEqualsICaseC(UTF8STRC("aBcDe1FgHiJl"), UTF8STRC("AbCdE1fGhIjK")))
	{
		return 1;
	}
	if (!Text::StrStartsWithICase((const UTF8Char*)"aBcDe1FgHiJk", (const UTF8Char*)"AbCdE1"))
	{
		return 1;
	}
	if (Text::StrStartsWithICase((const UTF8Char*)"aBcDe1FgHiJk", (const UTF8Char*)"AbCdE2"))
	{
		return 1;
	}
	if (!Text::StrStartsWithICaseC(UTF8STRC("aBcDe1FgHiJk"), UTF8STRC("AbCdE1")))
	{
		return 1;
	}
	if (Text::StrStartsWithICaseC(UTF8STRC("aBcDe1FgHiJk"), UTF8STRC("AbCdE2")))
	{
		return 1;
	}
	if (!Text::StrEndsWithICaseC(UTF8STRC("aBcDe1FgHiJk"), UTF8STRC("fGhIjK")))
	{
		return 1;
	}
	if (Text::StrEndsWithICaseC(UTF8STRC("aBcDe1FgHiJk"), UTF8STRC("fGhIjL")))
	{
		return 1;
	}
	if (Text::StrIndexOfCharC(UTF8STRC("abcde/defgh/akfsd"), '/') != 5)
	{
		return 1;
	}
	if (Text::StrIndexOfChar((const UTF8Char*)"abcde/defgh/akfsd", '/') != 5)
	{
		return 1;
	}
	if (Text::StrLastIndexOfCharC(UTF8STRC("abcde/defgh/akfsd"), '/') != 11)
	{
		return 1;
	}
	if (Text::StrLastIndexOfChar((const UTF8Char*)"abcde/defgh/akfsd", '/') != 11)
	{
		return 1;
	}
	// Text::StrIndexOfICase
	// Text::StrReplaceICase
	// Text::StrCompareICase
	return 0;
}
