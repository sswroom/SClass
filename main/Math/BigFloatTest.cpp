#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/BigFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	UOSInt i = 1;
	Math::BigFloat bf(256, CSTR("1.1"));
	Math::BigFloat bf2(256, 1);
	bf2.ToString(sb);
	console.WriteLine(sb.ToCString());
	while (i < 30)
	{
		sb.ClearStr();
		sb.Append(CSTR("1.1"));
		sb.Append(CSTR(" ^ "));
		sb.AppendUOSInt(i);
		sb.Append(CSTR(" = "));
		bf2 *= bf;
		bf2.ToString(sb);
		console.WriteLine(sb.ToCString());
		i++;
	}
	return 0;
}
