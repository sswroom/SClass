#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[32];
	UInt32 i = 100000;
	UInt32 j;
	Bool succ = true;
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	while (i-- > 0)
	{
		Text::StrHexVal32V(sbuff, i);
		j = (UInt32)Text::StrHex2Int32C(sbuff);
		if (i != j)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendU32(i);
			sb.AppendC(UTF8STRC(" != "));
			sb.AppendU32(j);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			succ = false;
			break;
		}
	}

	if (succ)
	{
		console->WriteLineC(UTF8STRC("Success"));
	}
	DEL_CLASS(console);
	return 0;
}
