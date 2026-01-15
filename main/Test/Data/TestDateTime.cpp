#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	dt.SetValue(CSTR("2020-12-31T16:01:02.123+04:00"));
	sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
	if (!Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("2020-12-31 16:01:02.123 +0400")))
	{
		return 1;
	}

	dt.SetTicks(1642509810123);
	dt.ToUTCTime();
	sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
	if (!Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("2022-01-18 12:43:30.123 +0000")))
	{
		return 1;
	}

	return 0;
}
