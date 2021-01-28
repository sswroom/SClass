#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	console.WriteLine((const UTF8Char*)"Hello World");
	return 0;
}
