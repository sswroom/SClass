#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter console;
	while (true)
	{
		sptr = IO::Console::GetLine(sbuff);
		if (Text::StrEquals(sbuff, (const UTF8Char*)"q"))
		{
			break;
		}
		console.WriteLine(CSTRP(sbuff, sptr));
	}
	return 0;
}
