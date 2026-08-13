#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Java/JavaClass.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Optional<IO::Java::JavaClass> cls = IO::Java::JavaClass::ParseFile(CSTR("Test.class"));
	NN<IO::Java::JavaClass> nncls;
	console.WriteLine(CSTR("ParseFile Returned\r\n"));
	if (cls.SetTo(nncls))
	{
		Text::StringBuilderUTF8 sb;
		nncls->FileStructDetail(sb);
		console.WriteLine(sb.ToCString());
	}
	cls.Delete();
	return 0;
}
