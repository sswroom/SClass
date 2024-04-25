#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

#define DIFFRATIO 0.000000001

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	if (!Math::NearlyEquals(Math_Ln(10.0), 2.302585092994, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_MyLn(10.0), 2.302585092994, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_Log10(59.0), 1.7708520116421, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_MyLog10(59.0), 1.7708520116421, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_Pow(10.0, -4.0), 0.0001, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_MyPow(10.0, -4.0), 0.0001, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_Pow(10.0, -8.0), 1.0e-8, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_MyPow(10.0, -8.0), 1.0e-8, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_Pow(10.0, 3.0), 1000, DIFFRATIO))
	{
		return 1;
	}
	if (!Math::NearlyEquals(Math_MyPow(10.0, 3.0), 1000, DIFFRATIO))
	{
		return 1;
	}

	sptr = Text::StrDoubleFmt(sbuff, 123456789.0, "#,###.000");
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("123,456,789.000")))
	{
		return 1;
	}
	return 0;
}
