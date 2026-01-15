#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	UIntOS i;
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < argc)
	{
		sb.ClearStr();
		sb.AppendUIntOS(i);
		sb.AppendUTF8Char('\t');
		sb.AppendSlow(UnsafeArray<const UTF8Char>(argv[i]));
		console.WriteLine(sb.ToCString());
		i++;
	}
	return 0;
}
