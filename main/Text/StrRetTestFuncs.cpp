#include "Stdafx.h"
#include "Text/CString.h"

const UTF8Char *Func1();
Text::CString Func2();
Text::CString Func2b();

UTF8Char *Func3(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, Func1());
}

UTF8Char *Func4(UTF8Char *sbuff)
{
	Text::CString s = Func2();
	return Text::StrConcatC(sbuff, s.v, s.leng);
}

UTF8Char *Func4b(UTF8Char *sbuff)
{
	Text::CString s = Func2b();
	return Text::StrConcatC(sbuff, s.v, s.leng);
}
