#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/JavaClass.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::JavaClass *cls = IO::JavaClass::ParseFile(CSTR("Test.class"));
	console.WriteLine(CSTR("ParseFile Returned\r\n"));
	if (cls)
	{
		Text::StringBuilderUTF8 sb;
		cls->FileStructDetail(sb);
		console.WriteLine(sb.ToCString());
	}
	SDEL_CLASS(cls);
	return 0;
}
