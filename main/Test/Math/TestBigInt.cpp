#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/BigIntLSB.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
//	IO::ConsoleWriter console;
	UOSInt i = 1;
	UOSInt j = 300;
	UOSInt k;
	Text::StringBuilderUTF8 sb;
	Math::BigIntLSB bi(256);
	Math::BigIntLSB bi2(256);
	bi = i;
	while (i < j)
	{
		bi *= i;
		sb.ClearStr();
		sb.AppendUOSInt(i)->Append(CSTR("! = "));
		k = sb.leng;
		bi.ToString(sb);
//		console.WriteLine(sb.ToCString());
		bi2 = sb.ToCString().Substring(k);
		if (bi != bi2)
		{
/*			sb.ClearStr();
			bi.ToString(sb);
			sb.Append(CSTR(" != "));
			bi2.ToString(sb);
			console.WriteLine(sb.ToCString());*/
			return 1;
		}
		i++;
	}
	bi.SetFactorial(299);
	i = 300;
	while (i-- > 1)
	{
		bi /= i;
	}
	bi2 = 1;
	if (bi != bi2)
	{
/*		sb.ClearStr();
		bi.ToString(sb);
		sb.Append(CSTR(" != "));
		bi2.ToString(sb);
		console.WriteLine(sb.ToCString());*/
		return 1;
	}
	return 0;
}
