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
	dt->ToLocalTime();
	if (v < 61)
	{
		v += 1;
	}
	Int32 inum = (Int32)v;
	Int32 ms;
	Int32 s;
	Int32 m;
	dt->SetValue(1899, 12, 30, 0, 0, 0, 0);
	dt->AddDay(inum);
	v -= inum;
	inum = Math::Double2Int32(v * 86400000.0);
	ms = inum % 1000;
	inum = inum / 1000;
	s = inum % 60;
	inum = inum / 60;
	m = inum % 60;
	inum = inum / 60;
	dt->SetValue(dt->GetYear(), dt->GetMonth(), dt->GetDay(), inum, m, s, ms);

/*	Int32 days = (Int32)v;
	Int8 tz;
	dt->ToLocalTime();
	tz = dt->GetTimeZoneQHR();
	dt->SetTicks((days - 25569) * 86400000LL + Math::Double2OSInt((v - days) * 86400000));
	dt->SetTimeZoneQHR(tz);*/
}
