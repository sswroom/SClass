#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("a"));
	sb.AppendUTF8Char('b');
	return 0;
}
