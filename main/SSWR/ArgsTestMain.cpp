#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < argc)
	{
		sb.ClearStr();
		sb.AppendUOSInt(i);
		sb.AppendUTF8Char('\t');
		sb.AppendSlow(argv[i]);
		console.WriteLine(sb.ToCString());
		i++;
	}
	return 0;
}
