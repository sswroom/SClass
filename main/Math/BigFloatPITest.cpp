#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/BigFloat.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[2048];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::ConsoleWriter> console;
	NN<Math::BigFloat> val;
	NN<Math::BigFloat> val2;
	
	NEW_CLASSNN(console, IO::ConsoleWriter());
	NEW_CLASSNN(val, Math::BigFloat(256));
	NEW_CLASSNN(val2, Math::BigFloat(256));

	val->SetPI();

	console->Write(CSTR("PI = "));
	sptr = val->ToString(sbuff);
	console->WriteLine(CSTRP(sbuff, sptr));


	val2.Delete();
	val.Delete();

	console.Delete();
	return 0;
}
