#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[32];
	UInt64 i;
	UInt64 j;
	Bool succ = true;
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	i = 100000;
	while (i-- > 0)
	{
		Text::StrHexVal64(sbuff, i);
		j = (UInt64)Text::StrHex2Int64C(sbuff);
		if (i != j)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendU64(i);
			sb.AppendC(UTF8STRC(" != "));
			sb.AppendU64(j);
			console->WriteLine(sb.ToString());
			succ = false;
			break;
		}
	}
	if (succ)
	{
		i = 10000100000LL;
		while (i-- > 10000000000LL)
		{
			Text::StrHexVal64(sbuff, i);
			j = (UInt64)Text::StrHex2Int64C(sbuff);
			if (i != j)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendU64(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendU64(j);
				console->WriteLine(sb.ToString());
				succ = false;
				break;
			}
		}
	}
	if (succ)
	{
		i = (UInt64)-100000;
		while (i++ != 0)
		{
			Text::StrHexVal64(sbuff, i);
			j = (UInt64)Text::StrHex2Int64C(sbuff);
			if (i != j)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendU64(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendU64(j);
				console->WriteLine(sb.ToString());
				succ = false;
				break;
			}
		}
	}
	if (succ)
	{
		i = (UInt64)-10000100000LL;
		while (i++ != (UInt64)-10000000000LL)
		{
			Text::StrHexVal64(sbuff, i);
			j = (UInt64)Text::StrHex2Int64C(sbuff);
			if (i != j)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendU64(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendU64(j);
				console->WriteLine(sb.ToString());
				succ = false;
				break;
			}
		}
	}


	if (succ)
	{
		console->WriteLine((const UTF8Char*)"Success");
	}
	DEL_CLASS(console);
	return 0;
}
