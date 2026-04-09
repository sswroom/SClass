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
	Math::BigFloat *val;
	UIntOS i = 1;

	NEW_CLASS(console, IO::ConsoleWriter());

	NEW_CLASS(val, Math::BigFloat(512));
	while (i < 10)
	{
		val->Factorial(i);
		sptr = Text::StrConcatC(Text::StrUIntOS(sbuff, i), UTF8STRC("! = "));
		console->Write(CSTRP(sbuff, sptr));
		sptr = val->ToString(sbuff);
		console->WriteLine(CSTRP(sbuff, sptr));
		i++;
	}
	DEL_CLASS(val);

	DEL_CLASS(console);
	return 0;
}
