#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
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
		console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	return 0;
}
