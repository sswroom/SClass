#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/Process.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	console.WriteLineC(UTF8STRC("Start Execute"));
	Manage::Process::ExecuteProcess(UTF8STRC("getprop"), &sb);
	console.WriteLineC(UTF8STRC("End Execute"));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	return 0;
}
