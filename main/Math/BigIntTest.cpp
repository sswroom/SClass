#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/BigIntLSB.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

void Test()
{
	UTF8Char buff[256];
	Math::BigIntLSB bi(64, CSTR("-237547064336589643472626930138252565981010257930"));
	bi.ToString(buff);
	printf("%s\n", buff);
	bi.ToHex(buff);
	printf("%s\n", buff);
	Int32 i = 10;
	while (i-- > 0)
	{
		bi /= 2;
		bi.ToString(buff);
		printf("%s\n", buff);
	}
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[2048];
	UTF8Char *sptr;
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Math::BigIntLSB val1(512);
	Math::BigIntLSB val2(512);

	Test();

	val1 = 1000000000;
	console.WriteLineC(UTF8STRC("Assign Int 1000000000"));
	sptr = val1.ToString(sbuff);
	console.WriteStrC(UTF8STRC("ToString "));
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

	val1.Neg();
	sptr = val1.ToString(sbuff);
	console.WriteStrC(UTF8STRC("Neg "));
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

	val2 = -1000000000;
	console.WriteLineC(UTF8STRC("Assign Int -1000000000"));
	sptr = val2.ToString(sbuff);
	console.WriteStrC(UTF8STRC("ToString "));
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

	val2 *= 1234567890;
	console.WriteStrC(UTF8STRC("-1000000000 * 1234567890 = "));
	sptr = val2.ToString(sbuff);
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

	console.WriteStrC(UTF8STRC("520! = "));
	if (val1.SetFactorial(520))
	{
		console.WriteStrC(UTF8STRC("(Overflow) "));
	}
	sptr = val1.ToString(sbuff);
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

	UInt32 i = 520;
	console.WriteStrC(UTF8STRC("Divide test: "));
	while (i > 1)
	{
		if (val1.DivideBy(i) != 0)
		{
			console.WriteLineC(UTF8STRC("Failed"));
			break;
		}
		i--;
	}
	if (i == 1)
	{
		if (val1.EqualsToI32(1))
		{
			console.WriteLineC(UTF8STRC("Success"));
		}
		else
		{
			console.WriteLineC(UTF8STRC("Failed"));
		}
	}
	
	return 0;
}
