#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/CString.h"
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

Text::CString Func2b()
{
	return Text::CString(UTF8STRC("Func3"));
}

UTF8Char *Func3(UTF8Char *sbuff);
UTF8Char *Func4(UTF8Char *sbuff);
UTF8Char *Func4b(UTF8Char *sbuff);

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[128];
	Func3(sbuff);
	printf("A: %s\r\n", sbuff);
	Func4(sbuff);
	printf("B: %s\r\n", sbuff);
	Func4b(sbuff);
	printf("C: %s\r\n", sbuff);
	return 0;
}

