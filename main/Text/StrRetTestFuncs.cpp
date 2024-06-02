#include "Stdafx.h"
#include "Text/CString.h"

const UTF8Char *Func1();
Text::CStringNN Func2();
Text::CStringNN Func2b();

UnsafeArray<UTF8Char> Func3(UnsafeArray<UTF8Char> sbuff)
{
	return Text::StrConcat(sbuff, Func1());
}

UnsafeArray<UTF8Char> Func4(UnsafeArray<UTF8Char> sbuff)
{
	Text::CStringNN s = Func2();
	return Text::StrConcatC(sbuff, s.v, s.leng);
}

UnsafeArray<UTF8Char> Func4b(UnsafeArray<UTF8Char> sbuff)
{
	Text::CStringNN s = Func2b();
	return Text::StrConcatC(sbuff, s.v, s.leng);
}
