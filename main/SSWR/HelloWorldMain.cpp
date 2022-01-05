#include "Stdafx.h"
#include "Core/Core.h"
#ifdef DEBUGCON
#include "IO/DebugWriter.h"
#else
#include "IO/ConsoleWriter.h"
#endif
#include "IO/Path.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
#ifdef DEBUGCON
	IO::DebugWriter console;
#else
	IO::ConsoleWriter console;
#endif
	console.WriteLine(UTF8STRC("Hello World"));
	UTF8Char sbuff[512];
	UTF8Char *sptr = IO::Path::GetProcessFileName(sbuff);
	console.WriteLine(sbuff, (UOSInt)(sptr - sbuff));
	return 0;
}
