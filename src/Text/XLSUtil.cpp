#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Text/XLSUtil.h"

Double Text::XLSUtil::Date2Number(NN<Data::DateTime> dt)
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
	return days + (Double)ticks / 86400000.0 + 50000 / 86400000000000.0;
}

Double Text::XLSUtil::Date2Number(Data::Timestamp ts)
{
	ts = ts.SetTimeZoneQHR(0);
	Int64 secs = ts.inst.sec;
	Int32 days = (Int32)(secs / 86400LL) + 25569;
	secs -= (days - 25569) * 86400;
	while (secs < 0)
	{
		secs += 86400LL;
		days -= 1;
	}
	return days + (Double)secs / 86400.0 + (ts.inst.nanosec + 50000) / 86400000000000.0;
}

void Text::XLSUtil::Number2Date(NN<Data::DateTime> dt, Double v)
{
	Int32 days = (Int32)v;
	Int8 tz;
	dt->ToLocalTime();
	tz = dt->GetTimeZoneQHR();
	dt->SetTicks((days - 25569) * 86400000LL + Double2OSInt((v - days) * 86400000));
	dt->SetTimeZoneQHR(tz);
}

Data::Timestamp Text::XLSUtil::Number2Timestamp(Double v)
{
	Int32 days = (Int32)v;
	Int8 tz = Data::DateTimeUtil::GetLocalTzQhr();
	Double ds = (v - days);
	OSInt s = (OSInt)(ds * 86400);
	return Data::Timestamp(Data::TimeInstant((days - 25569) * 86400LL + s, (UInt32)((ds * 86400 - (Double)s) * 1000000000)), tz);
}

UnsafeArray<UTF8Char> Text::XLSUtil::GetCellID(UnsafeArray<UTF8Char> sbuff, UOSInt col, UOSInt row)
{
	if (col >= 26)
	{
		*sbuff++ = (UTF8Char)('A' + (col / 26) - 1);
		*sbuff++ = (UTF8Char)('A' + (col % 26));
	}
	else
	{
		*sbuff++ = (UTF8Char)('A' + col);
	}
	sbuff = Text::StrUOSInt(sbuff, row + 1);
	return sbuff;
}
