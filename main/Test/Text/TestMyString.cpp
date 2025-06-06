#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
//	UTF8Char sbuff[64];
//	UTF8Char *sptr;
	UOSInt i;
	i = Text::StrCharCnt((const UTF8Char*)"aBcDeFgHiJklmnopqrstuvwxyz");
	if (i != 26)
	{
		return 1;
	}

	i = Text::StrIndexOfChar((const UTF8Char*)"aBcDeFgHiJklmnopqrstuvwxyz", 'z');
	if (i != 25)
	{
		return 1;
	}

	i = Text::StrIndexOfChar((const UTF8Char*)"aBcDeFgHiJklmnopqrstuvwxyz", 'Z');
	if (i != INVALID_INDEX)
	{
		return 1;
	}

	i = Text::StrIndexOfCharC(UTF8STRC("aBcDeFgHiJklmnopqrstuvwxyz"), 'z');
	if (i != 25)
	{
		return 1;
	}

	i = Text::StrIndexOfCharC(UTF8STRC("aBcDeFgHiJklmnopqrstuvwxyz"), 'Z');
	if (i != INVALID_INDEX)
	{
		return 1;
	}


	return 0;
}
