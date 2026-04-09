#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/BigFloat.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[2048];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter *console;
	NN<Math::BigFloat> val;
	NN<Math::BigFloat> val2;
	UInt32 i;
	Bool neg = false;

	NEW_CLASS(console, IO::ConsoleWriter());

	NEW_CLASSNN(val, Math::BigFloat(256));
	NEW_CLASSNN(val2, Math::BigFloat(256));

	*val2.Ptr() = 1;

	val->SetE(val2);
	console->Write(CSTR("e = "));
	sptr = val->ToString(sbuff);
	console->WriteLine(CSTRP(sbuff, sptr));

	val->SetSin(val2);
	console->Write(CSTR("sin = "));
	sptr = val->ToString(sbuff);
	console->WriteLine(CSTRP(sbuff, sptr));

	val->SetCos(val2);
	console->Write(CSTR("cos = "));
	sptr = val->ToString(sbuff);
	console->WriteLine(CSTRP(sbuff, sptr));

	val->SetTan(val2);
	console->Write(CSTR("tan = "));
	sptr = val->ToString(sbuff);
	console->WriteLine(CSTRP(sbuff, sptr));

	val2.Delete();
	val.Delete();

	DEL_CLASS(console);
	return 0;
}
