#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[512];
	IO::ConsoleWriter console;
	while (IO::Console::GetLine(sbuff))
	{
		if (Text::StrEquals(sbuff, (const UTF8Char*)"q"))
		{
			break;
		}
		console.WriteLine(sbuff);
	}
	return 0;
}
