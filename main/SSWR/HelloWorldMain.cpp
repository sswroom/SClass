#include "Stdafx.h"
#include "Core/Core.h"
#ifdef DEBUGCON
#include "IO/DebugWriter.h"
#else
#include "IO/ConsoleWriter.h"
#endif

Int32 MyMain(Core::IProgControl *progCtrl)
{
#ifdef DEBUGCON
	IO::DebugWriter console;
#else
	IO::ConsoleWriter console;
#endif
	console.WriteLine((const UTF8Char*)"Hello World");
	return 0;
}
