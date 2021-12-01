#include "Stdafx.h"
#include "Math/Math.h"
#include "Text/XLSUtil.h"

Double Text::XLSUtil::Date2Number(Data::DateTime *dt)
{
	dt->ToLocalTime();
	dt->SetTimeZoneQHR(0);
	Int64 ticks = dt->ToTicks();
	Int32 days = (Int32)(ticks / 86400000LL) + 25569;
	ticks -= (days - 25569) * 86400000LL;
	while (ticks < 0)
	{
		ticks += 86400000LL;
		days -= 1;
	}
	return days + (Double)ticks / 86400000.0;
}

void Text::XLSUtil::Number2Date(Data::DateTime *dt, Double v)
{
	Int32 days = (Int32)v;
	Int8 tz;
	dt->ToLocalTime();
	tz = dt->GetTimeZoneQHR();
	dt->SetTicks((days - 25569) * 86400000LL + Math::Double2OSInt((v - days) * 86400000));
	dt->SetTimeZoneQHR(tz);
}
