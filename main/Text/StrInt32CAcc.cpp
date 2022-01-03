#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Char sbuff[32];
	Int32 i = 100000000;
	Int32 j;
	Bool succ = true;
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	while (i-- > 0)
	{
		Text::StrInt32(sbuff, i);
		j = Text::StrToInt32(sbuff);
		if (i != j)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendI32(i);
			sb.AppendC(UTF8STRC(" != "));
			sb.AppendI32(j);
			console->WriteLine(sb.ToString());
			succ = false;
			break;
		}
	}

	if (succ)
	{
		console->WriteLine((const UTF8Char*)"Success");
	}
	DEL_CLASS(console);
	return 0;
}
