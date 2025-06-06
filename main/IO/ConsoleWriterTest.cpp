#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	console.WriteLine(CSTR("WriteLine Test"));
	console.WriteLine(CSTR("Second Line"));
	console.WriteLine();
	console.WriteLine(CSTR("Mixed writeline\r\nMixed Second Line"));
	return 0;
}
