#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"

#include <stdio.h>

const UTF8Char *Func1()
{
	return (const UTF8Char *)"Func1";
}

Text::CString Func2()
{
	return {UTF8STRC("Func2")};
}

UTF8Char *Func3(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, Func1());
}

UTF8Char *Func4(UTF8Char *sbuff)
{
	Text::CString s = Func2();
	return Text::StrConcatC(sbuff, s.v, s.len);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[128];
	Func3(sbuff);
	printf("A: %s\r\n", sbuff);
	Func4(sbuff);
	printf("B: %s\r\n", sbuff);
	return 0;
}
