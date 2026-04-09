#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/BigFloat.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[2048];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());

	Math::BigFloat val(512);
	Math::BigFloat val2(512);
	val = CSTR("1");
	val2 = CSTR("3.333333333333333333333333");

	sptr = Text::StrConcatC(val2.ToString(Text::StrConcatC(val.ToString(sbuff), UTF8STRC(" / "))), UTF8STRC(" = "));
	val /= val2;

	console->Write(CSTRP(sbuff, sptr));
	sptr = val.ToString(sbuff);
	console->WriteLine(CSTRP(sbuff, sptr));

	DEL_CLASS(console);
	return 0;
}
