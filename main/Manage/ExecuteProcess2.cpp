#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/Process.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	console.WriteLine(CSTR("Start Execute"));
	Manage::Process::ExecuteProcess(CSTR("getprop"), sb);
	console.WriteLine(CSTR("End Execute"));
	console.WriteLine(sb.ToCString());
	return 0;
}
