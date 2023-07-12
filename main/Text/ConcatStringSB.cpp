#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	OSInt i;
	OSInt j;
	Manage::HiResClock clk;
	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		Text::StringBuilderUTF8 sb;
		j = 1000001000;
		while (j-- > 1000000000)
		{
			sb.AppendC(UTF8STRC("1234567890"));
		}
	}
	Double t1 = clk.GetTimeDiff();
	IO::ConsoleWriter console;
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, t1), UTF8STRC(" s"));
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

	return 0;
}
