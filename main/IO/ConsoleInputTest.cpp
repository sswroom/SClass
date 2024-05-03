#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::ConsoleWriter console;
	while ((sptr = IO::Console::GetLine(sbuff)) != 0)
	{
		if (Text::StrEquals(sbuff, (const UTF8Char*)"q"))
		{
			break;
		}
		console.WriteLine(CSTRP(sbuff, sptr));
	}
	return 0;
}
