#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/Math.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include <wchar.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	console.WriteLineC(UTF8STRC("Ln(10) = 2.302585092994"));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("  Calc = "));
	Text::SBAppendF64(&sb, Math_Ln(10.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	sb.ClearStr();
	sb.AppendC(UTF8STRC(" Calc2 = "));
	Text::SBAppendF64(&sb, Math_MyLn(10.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	console.WriteLine();

	console.WriteLineC(UTF8STRC("Log10(59) = 1.7708520116421"));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("     Calc = "));
	Text::SBAppendF64(&sb, Math_Log10(59.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("    Calc2 = "));
	Text::SBAppendF64(&sb, Math_MyLog10(59.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	console.WriteLine();

	console.WriteLineC(UTF8STRC("10 ^ -4 = 1.0e-4"));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("   Calc = "));
	Text::SBAppendF64(&sb, Math_Pow(10.0, -4.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("  Calc2 = "));
	Text::SBAppendF64(&sb, Math_MyPow(10.0, -4.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	console.WriteLine();

	console.WriteLineC(UTF8STRC("10 ^ -8 = 1.0e-8"));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("   Calc = "));
	Text::SBAppendF64(&sb, Math_Pow(10.0, -8.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("  Calc2 = "));
	Text::SBAppendF64(&sb, Math_MyPow(10.0, -8.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	console.WriteLine();

	console.WriteLineC(UTF8STRC("10 ^ 3 = 1000"));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("  Calc = "));
	Text::SBAppendF64(&sb, Math_Pow(10.0, 3.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	sb.ClearStr();
	sb.AppendC(UTF8STRC(" Calc2 = "));
	Text::SBAppendF64(&sb, Math_MyPow(10.0, 3.0));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	console.WriteLine();

	sptr = Text::StrDoubleFmt(sbuff, 123456789.0, "#,###.000");
	console.WriteStrC(UTF8STRC("StrDoubleFmt = "));
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	return 0;
}
