#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/BigFloat.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	NN<Math::BigFloat> val;
	NN<Math::BigFloat> val2;
	NN<IO::ConsoleWriter> console;
	NEW_CLASSNN(val, Math::BigFloat(256));
	NEW_CLASSNN(val2, Math::BigFloat(256));
	NEW_CLASSNN(console, IO::ConsoleWriter());

	*val2.Ptr() = CSTR("0.000000000000000000000001");
	console->WriteLine(CSTR("Step 1"));
	val->SetLn(val2);
	console->WriteLine(CSTR("Step 2"));
	sptr = val->ToString(sbuff);
	console->WriteLine(CSTRP(sbuff, sptr));

	console.Delete();
	val2.Delete();
	val.Delete();
	return 0;
}