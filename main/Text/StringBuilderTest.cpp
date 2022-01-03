#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("a"));
	sb.AppendChar('b', 1);
	return 0;
}
