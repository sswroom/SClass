#include "Stdafx.h"
#include "Core/Core.h"
#ifdef DEBUGCON
#include "IO/DebugWriter.h"
#else
#include "IO/ConsoleWriter.h"
#endif
#include "IO/Path.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
#ifdef DEBUGCON
	IO::DebugWriter console;
#else
	IO::ConsoleWriter console;
#endif
	console.WriteLine(CSTR("Hello World"));
	UTF8Char sbuff[512];
	sbuff[0] = 0;
	UnsafeArray<UTF8Char> sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	console.WriteLine(CSTRP(sbuff, sptr));
	return 0;
}
