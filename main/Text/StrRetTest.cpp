#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/CString.h"
#include "Text/MyString.h"

#include <stdio.h>

const UTF8Char *Func1()
{
	return (const UTF8Char*)"Func1";
}

Text::CStringNN Func2()
{
	return CSTR("Func2");
}

Text::CStringNN Func2b()
{
	return Text::CStringNN(UTF8STRC("Func3"));
}

UnsafeArray<UTF8Char> Func3(UnsafeArray<UTF8Char> sbuff);
UnsafeArray<UTF8Char> Func4(UnsafeArray<UTF8Char> sbuff);
UnsafeArray<UTF8Char> Func4b(UnsafeArray<UTF8Char> sbuff);

Int32 MyMain(NN<Core::IProgControl> progCtrl)
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

