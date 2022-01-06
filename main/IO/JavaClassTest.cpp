#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/JavaClass.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	IO::JavaClass *cls = IO::JavaClass::ParseFile((const UTF8Char*)"Test.class");
	console.WriteLineC(UTF8STRC("ParseFile Returned\r\n"));
	if (cls)
	{
		Text::StringBuilderUTF8 sb;
		cls->FileStructDetail(&sb);
		console.WriteLineC(sb.ToString(), sb.GetLength());
	}
	SDEL_CLASS(cls);
	return 0;
}
