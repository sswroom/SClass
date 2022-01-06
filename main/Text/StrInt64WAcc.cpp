#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WChar sbuff[32];
	Int64 i;
	Int64 j;
	Bool succ = true;
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	i = 100000;
	while (i-- > 0)
	{
		Text::StrInt64(sbuff, i);
		j = Text::StrToInt64(sbuff);
		if (i != j)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendI64(i);
			sb.AppendC(UTF8STRC(" != "));
			sb.AppendI64(j);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			succ = false;
			break;
		}
	}
	if (succ)
	{
		i = 10000100000LL;
		while (i-- > 10000000000LL)
		{
			Text::StrInt64(sbuff, i);
			j = Text::StrToInt64(sbuff);
			if (i != j)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendI64(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendI64(j);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				succ = false;
				break;
			}
		}
	}
	if (succ)
	{
		i = 0x100010000LL;
		while (i-- > 0xffff0000LL)
		{
			Text::StrInt64(sbuff, i);
			j = Text::StrToInt64(sbuff);
			if (i != j)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendI64(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendI64(j);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				succ = false;
				break;
			}
		}
	}
/*	if (succ)
	{
		i = -100000;
		while (i++ < 0)
		{
			Text::StrInt64(sbuff, i);
			j = Text::StrToInt64(sbuff);
			if (i != j)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendI64(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendI64(j);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				succ = false;
				break;
			}
		}
	}
	if (succ)
	{
		i = -10000100000LL;
		while (i++ < -10000000000LL)
		{
			Text::StrInt64(sbuff, i);
			j = Text::StrToInt64(sbuff);
			if (i != j)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendI64(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendI64(j);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				succ = false;
				break;
			}
		}
	}*/


	if (succ)
	{
		console->WriteLineC(UTF8STRC("Success"));
	}
	DEL_CLASS(console);
	return 0;
}
