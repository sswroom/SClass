#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < argc)
	{
		sb.ClearStr();
		sb.AppendUOSInt(i);
		sb.AppendChar('\t', 1);
		sb.Append(argv[i]);
		console.WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	return 0;
}
