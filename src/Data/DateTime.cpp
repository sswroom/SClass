#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#if defined(_WIN32) || defined(_WIN32_WCE)
#include <windows.h>
#else
typedef struct
{
	UInt16 wYear;
	UInt16 wMonth;
	UInt16 wDayOfWeek;
	UInt16 wDay;
	UInt16 wHour;
	UInt16 wMinute;
	UInt16 wSecond;
	UInt16 wMilliseconds;
} SYSTEMTIME;
#if !defined(CPU_AVR)
#include <sys/time.h>
#endif
#endif
#include <time.h>
#include <stdio.h>

NN<Data::DateTimeUtil::TimeValue> Data::DateTime::GetTimeValue()
{
	NN<Data::DateTimeUtil::TimeValue> t = this->val.t;
	switch (this->timeType)
	{
	case TimeType::Time:
		return t;
	case TimeType::Ticks:
		this->timeType = TimeType::Time;
		Data::DateTimeUtil::Secs2TimeValue(this->val.secs, t, this->tzQhr);
		return t;
	case TimeType::None:
	default:
		this->timeType = TimeType::Time;
		t->year = 1970;
		t->month = 1;
		t->day = 1;
		t->hour = 0;
		t->minute = 0;
		t->second = 0;
		return t;		
	}
}

void Data::DateTime::FixValues()
{
	NN<Data::DateTimeUtil::TimeValue> t = GetTimeValue();
	while (this->ns >= 1000000000)
	{
		this->ns = (this->ns - 1000000000);
		t->second++;
	}
	while (t->second >= 60)
	{
		t->second = (UInt8)(t->second - 60);
		t->minute++;
	}
	while (t->minute >= 60)
	{
		t->minute = (UInt8)(t->minute - 60);
		t->hour++;
	}
	while (t->hour >= 24)
	{
		t->hour = (UInt8)(t->hour - 24);
		t->day++;
	}
	while (t->day < 1)
	{
		t->month--;
		t->day = (UInt8)(t->day + Data::DateTimeUtil::DayInMonth(t->year, t->month));
	}
	UInt32 i;
	while (t->day > (i = Data::DateTimeUtil::DayInMonth(t->year, t->month)))
	{
		t->day = (UInt8)(t->day - i);
		t->month++;
	}
	while (t->month < 1)
	{
		t->month = (UInt8)(t->month + 12);
		t->year--;
	}
	while (t->month > 12)
	{
		t->month = (UInt8)(t->month - 12);
		t->year++;
	}
}

Data::DateTime::DateTime()
{
	this->timeType = TimeType::None;
	this->ns = 0;
	this->tzQhr = 0;
}

Data::DateTime::DateTime(Int64 ticks)
{
	this->SetValue(ticks, 0);
}

Data::DateTime::DateTime(Int64 ticks, Int8 tzQhr)
{
	this->SetValue(ticks, tzQhr);
}

Data::DateTime::DateTime(const Data::TimeInstant &instant, Int8 tzQhr)
{
	this->SetValue(instant, tzQhr);
}

Data::DateTime::DateTime(Int32 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second)
{
	this->timeType = TimeType::Time;
	this->tzQhr = 0;
	Data::DateTimeUtil::TimeValue *t = &this->val.t;
	t->year = year;
	t->month = month;
	t->day = day;
	t->hour = hour;
	t->minute = minute;
	t->second = second;
	this->ns = 0;
}

Data::DateTime::DateTime(Int32 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms)
{
	this->timeType = TimeType::Time;
	this->tzQhr = 0;
	Data::DateTimeUtil::TimeValue *t = &this->val.t;
	t->year = year;
	t->month = month;
	t->day = day;
	t->hour = hour;
	t->minute = minute;
	t->second = second;
	this->ns = (UInt32)ms * 1000000;
}

Data::DateTime::DateTime(Text::CStringNN dateStr)
{
	this->timeType = TimeType::None;
	this->ns = 0;
	this->tzQhr = 0;
	this->SetValue(dateStr);
}

Data::DateTime::DateTime(NN<Data::DateTime> dt)
{
	this->SetValue(dt);
}

Data::DateTime::~DateTime()
{
}

Bool Data::DateTime::SetAsComputerTime()
{
#ifdef WIN32
	NN<Data::DateTimeUtil::TimeValue> t = GetTimeValue();
	SYSTEMTIME st;
	ToUTCTime();
	st.wYear = t->year;
	st.wMonth = t->month;
	st.wDay = t->day;
	st.wHour = t->hour;
	st.wMinute = t->minute;
	st.wSecond = t->second;
	st.wMilliseconds = (UInt16)(this->ns / 1000000);
	return SetSystemTime(&st) != FALSE;
#elif !defined(CPU_AVR)
	struct timespec tp;
	tp.tv_sec = (time_t)(this->ToTicks() / 1000);
	tp.tv_nsec = 0;
	clock_settime(CLOCK_REALTIME, &tp);
	return clock_settime(CLOCK_REALTIME, &tp) == 0;
#else
	return false;
#endif
}

NN<Data::DateTime> Data::DateTime::SetCurrTime()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef WIN32
	NN<Data::DateTimeUtil::TimeValue> t = GetTimeValue();
	SYSTEMTIME st;
	TIME_ZONE_INFORMATION tz;
	GetLocalTime(&st);
	tz.Bias = 0;
	GetTimeZoneInformation(&tz);
	t->year = st.wYear;
	t->month = (UInt8)st.wMonth;
	t->day = (UInt8)st.wDay;
	t->hour = (UInt8)st.wHour;
	t->minute = (UInt8)st.wMinute;
	t->second = (UInt8)st.wSecond;
	this->ns = (UInt32)st.wMilliseconds * 1000000;
	this->tzQhr = (Int8)(-tz.Bias / 15);
#endif
	return *this;
#elif !defined(CPU_AVR)
	this->SetValue(Data::TimeInstant::Now(), Data::DateTimeUtil::GetLocalTzQhr());
	return *this;
#else
	return *this;
#endif
}

NN<Data::DateTime> Data::DateTime::SetCurrTimeUTC()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef WIN32
	SYSTEMTIME st;
	GetSystemTime(&st);
	NN<Data::DateTimeUtil::TimeValue> tval = GetTimeValue();
	tval->year = st.wYear;
	tval->month = (UInt8)st.wMonth;
	tval->day = (UInt8)st.wDay;
	tval->hour = (UInt8)st.wHour;
	tval->minute = (UInt8)st.wMinute;
	tval->second = (UInt8)st.wSecond;
	this->ns = (UInt32)st.wMilliseconds * 1000000;
	this->tzQhr = 0;
#endif
	return *this;
#elif !defined(CPU_AVR)
	this->SetValue(Data::TimeInstant::Now(), 0);
	return *this;
#else
	return *this;
#endif
}

void Data::DateTime::SetValue(NN<const DateTime> time)
{
	this->timeType = time->timeType;
	this->tzQhr = time->tzQhr;
	if (this->timeType == TimeType::Time)
	{
		this->val.t.year = time->val.t.year;
		this->val.t.month = time->val.t.month;
		this->val.t.day = time->val.t.day;
		this->val.t.hour = time->val.t.hour;
		this->val.t.minute = time->val.t.minute;
		this->val.t.second = time->val.t.second;
	}
	else
	{
		this->val.secs = time->val.secs;
	}
	this->ns = time->ns;
}

void Data::DateTime::SetValue(Int32 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms, Int8 tzQhr)
{
	this->SetValue(year, month, day, hour, minute, second, ms);
	this->tzQhr = tzQhr;
}

void Data::DateTime::SetValue(Int32 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms)
{
	this->SetDate(year, month, day);
	this->SetHour(hour);
	this->SetMinute(minute);
	second += ms / 1000;
	ms = ms % 1000;
	if (ms < 0)
	{
		ms += 1000;
		second -= 1;
	}
	this->SetSecond(second);
	this->ns = (UInt32)(ms * 1000000);
}

void Data::DateTime::SetValue(Int64 ticks, Int8 tzQhr)
{
	this->SetTicks(ticks);
	this->tzQhr = tzQhr;
}

void Data::DateTime::SetValue(const Data::TimeInstant &instant, Int8 tzQhr)
{
	this->SetInstant(instant);
	this->tzQhr = tzQhr;
}

void Data::DateTime::SetValueNoFix(Int32 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms, Int8 tzQhr)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	tval->year = year;
	tval->month = month;
	tval->day = day;
	tval->hour = hour;
	tval->minute = minute;
	tval->second = second;
	this->ns = (UInt32)ms * 1000000;
	this->tzQhr = tzQhr;	
}

Bool Data::DateTime::SetValueSlow(UnsafeArray<const Char> dateStr)
{
	return this->SetValue(Text::CStringNN(UnsafeArray<const UTF8Char>::ConvertFrom(dateStr), Text::StrCharCntCh(dateStr)));
}

Bool Data::DateTime::SetValue(Text::CStringNN dateStr)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	return Data::DateTimeUtil::String2TimeValue(dateStr, tval, this->tzQhr, this->tzQhr, this->ns);
}

void Data::DateTime::SetValueSYSTEMTIME(const void *sysTime)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	
	SYSTEMTIME *stime = (SYSTEMTIME*)sysTime;
	tval->year = stime->wYear;
	tval->month = (UInt8)stime->wMonth;
	tval->day = (UInt8)stime->wDay;
	tval->hour = (UInt8)stime->wHour;
	tval->minute = (UInt8)stime->wMinute;
	tval->second = (UInt8)stime->wSecond;
	this->ns = (UInt32)stime->wMilliseconds * 1000000;
	this->tzQhr = 0;
}

void Data::DateTime::SetValueFILETIME(const void *fileTime)
{
	UInt32 nanosecs;
	Int64 secs = Data::DateTimeUtil::FILETIME2Secs(fileTime, nanosecs);
	this->SetInstant(Data::TimeInstant(secs, nanosecs));
}

void Data::DateTime::SetValueVariTime(Double variTime)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	tval->year = 1900;
	tval->month = 1;
	tval->day = 1;
	tval->hour = 0;
	tval->minute = 0;
	tval->second = 0;
	this->tzQhr = 0;
	OSInt d = (OSInt)variTime;
	this->AddDay(d);
	variTime = (variTime - OSInt2Double(d)) * 24;
	tval->hour = (UInt8)variTime;
	variTime = (variTime - tval->hour) * 60;
	tval->minute = (UInt8)variTime;
	variTime = (variTime - tval->minute) * 60;
	tval->second = (UInt8)variTime;
	variTime = (variTime - tval->second) * 1000000000;
	this->ns = (UInt32)variTime;
}

Int32 Data::DateTime::GetYear()
{
	return this->GetTimeValue()->year;
}

UInt8 Data::DateTime::GetMonth()
{
	return this->GetTimeValue()->month;
}

UInt8 Data::DateTime::GetDay()
{
	return this->GetTimeValue()->day;
}

UInt8 Data::DateTime::GetHour()
{
	return this->GetTimeValue()->hour;
}

UInt8 Data::DateTime::GetMinute()
{
	return this->GetTimeValue()->minute;
}

UInt8 Data::DateTime::GetSecond()
{
	return this->GetTimeValue()->second;
}

UInt16 Data::DateTime::GetMS()
{
	return (UInt16)(this->ns / 1000000);
}

UInt32 Data::DateTime::GetNS()
{
	return this->ns;
}

NN<Data::DateTime> Data::DateTime::AddMonth(OSInt val)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	OSInt newMonth = tval->month + val;
	while (newMonth < 1)
	{
		newMonth += 12;
		tval->year--;
	}
	while (newMonth > 12)
	{
		newMonth -= 12;
		tval->year++;
	}
	tval->month = (UInt8)newMonth;
	UInt8 newDay = Data::DateTimeUtil::DayInMonth(tval->year, (UInt8)newMonth);
	if (tval->day > newDay)
	{
		tval->day = newDay;
	}
	return *this;
}

NN<Data::DateTime> Data::DateTime::AddDay(OSInt val)
{
	switch (this->timeType)
	{
	case TimeType::Time:
		{
			NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
			OSInt newDay = tval->day + val;
			OSInt dayim;
			if (newDay < 1)
			{
				while (newDay < 1)
				{
					if (--tval->month <= 0)
					{
						tval->year--;
						tval->month = (UInt8)(tval->month + 12);
					}
					newDay += Data::DateTimeUtil::DayInMonth(tval->year, tval->month);
				}
			}
			else
			{
				while (newDay > (dayim = Data::DateTimeUtil::DayInMonth(tval->year, tval->month)))
				{
					newDay = newDay - dayim;
					if (++tval->month > 12)
					{
						tval->year++;
						tval->month = (UInt8)(tval->month - 12);
					}
				}
			}
			tval->day = (UInt8)newDay;
		}
		break;
	case TimeType::None:
		this->timeType = TimeType::Ticks;
		this->val.secs = val * 86400LL;
		break;
	case TimeType::Ticks:
		this->val.secs += val * 86400LL;
		break;
	}
	return *this;
}

NN<Data::DateTime> Data::DateTime::AddHour(OSInt val)
{
	switch (this->timeType)
	{
	case TimeType::Time:
		{
			NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
			OSInt day = val / 24;
			OSInt outHour;
			outHour = val - day * 24 + tval->hour;
			while (outHour < 0)
			{
				outHour += 24;
				day--;
			}
			while (outHour >= 24)
			{
				outHour -= 24;
				day++;
			}
			tval->hour = (UInt8)outHour;
			if (day)
				AddDay(day);
		}
		break;
	case TimeType::Ticks:
		this->val.secs += val * 3600LL;
		break;
	case TimeType::None:
	default:
		this->val.secs = val * 3600LL;
		this->timeType = TimeType::Ticks;
		break;
	}
	return *this;
}

NN<Data::DateTime> Data::DateTime::AddMinute(OSInt val)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	OSInt hours = val / 60;
	OSInt outMin;
	outMin = val - hours * 60 + tval->minute;
	while (outMin < 0)
	{
		outMin += 60;
		hours--;
	}
	while (outMin >= 60)
	{
		outMin -= 60;
		hours++;
	}
	tval->minute = (UInt8)outMin;
	if (hours)
		AddHour(hours);
	return *this;
}

NN<Data::DateTime> Data::DateTime::AddSecond(OSInt val)
{
	if (this->timeType == TimeType::Time)
	{
		NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
		OSInt minutes = val / 60;
		OSInt outSec;
		outSec = val - minutes * 60 + tval->second;
		while (outSec < 0)
		{
			outSec += 60;
			minutes--;
		}
		while (outSec >= 60)
		{
			outSec -= 60;
			minutes++;
		}
		tval->second = (UInt8)outSec;
		if (minutes)
			AddMinute(minutes);
		return *this;
	}
	else
	{
		this->val.secs += (Int64)val;
		return *this;
	}
}

NN<Data::DateTime> Data::DateTime::AddMS(OSInt val)
{
	if (this->timeType == TimeType::Time)
	{
		OSInt seconds = val / 1000;
		Int64 outNS = (val - seconds * 1000) * 1000000 + this->ns;
		while (outNS < 0)
		{
			outNS += 1000000000;
			seconds--;
		}
		while (outNS >= 1000000000)
		{
			outNS -= 1000000000;
			seconds++;
		}
		this->ns = (UInt32)outNS;
		if (seconds)
			AddSecond(seconds);
	}
	else
	{
		OSInt seconds = val / 1000;
		val = val % 1000;
		Int32 newNS = (Int32)this->ns + (Int32)val * 1000000;
		if (newNS < 0)
		{
			newNS += 1000000000;
			seconds--;
		}
		else if (newNS >= 1000000000)
		{
			newNS -= 1000000000;
			seconds++;
		}
		this->ns = (UInt32)newNS;
		this->val.secs += seconds;
	}
	return *this;
}

void Data::DateTime::SetDate(Int32 year, OSInt month, OSInt day)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	tval->year = year;
	while (month < 1)
	{
		month += 12;
		tval->year--;
	}
	while (month > 12)
	{
		month -= 12;
		tval->year++;
	}
	tval->month = (UInt8)month;
	OSInt dayim;
	if (day < 1)
	{
		while (day < 1)
		{
			if (--tval->month <= 0)
			{
				tval->year--;
				tval->month = (UInt8)(tval->month + 12);
			}
			day += Data::DateTimeUtil::DayInMonth(tval->year, tval->month);
		}
	}
	else
	{
		while (day > (dayim = Data::DateTimeUtil::DayInMonth(tval->year, tval->month)))
		{
			day = day - dayim;
			if (++tval->month > 12)
			{
				tval->year++;
				tval->month = (UInt8)(tval->month - 12);
			}
		}
	}
	tval->day = (UInt8)day;
}

void Data::DateTime::SetYear(Int32 year)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	tval->year = year;
}

void Data::DateTime::SetMonth(OSInt month)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	this->SetDate(tval->year, month, tval->day);
}

void Data::DateTime::SetDay(OSInt day)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	this->SetDate(tval->year, tval->month, day);
}

void Data::DateTime::SetHour(OSInt hour)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	OSInt d = 0;
	if (hour < 0)
	{
		d = (hour / 24) - 1;
		hour = hour - d * 24;
	}
	else if (hour >= 24)
	{
		d = (hour / 24);
		hour = hour % 24;
	}
	tval->hour = (UInt8)hour;
	if (d != 0)
	{
		this->SetDate(tval->year, tval->month, tval->day + d);
	}
}

void Data::DateTime::SetMinute(OSInt minute)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	OSInt h = 0;
	if (minute < 0)
	{
		h = (minute / 60) - 1;
		minute = minute - h * 60;
	}
	else if (minute >= 60)
	{
		h = minute / 60;
		minute = minute % 60;
	}
	tval->minute = (UInt8)minute;
	if (h != 0)
	{
		this->SetHour(tval->hour + h);
	}
}

void Data::DateTime::SetSecond(OSInt second)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	OSInt m = 0;
	if (second < 0)
	{
		m = (second / 60) - 1;
		second = second - m * 60;
	}
	else if (second >= 60)
	{
		m = second / 60;
		second = second % 60;
	}
	tval->second = (UInt8)second;
	if (m != 0)
	{
		this->SetMinute(tval->minute + m);
	}
}

void Data::DateTime::SetNS(UInt32 ns)
{
	if (ns >= 1000000000)
	{
		this->AddSecond((OSInt)(ns / 1000000000));
		this->ns = ns % 1000000000;
	}
	else
	{
		this->ns = ns;
	}
}

/*void Data::DateTime::SetMS(OSInt ms)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	OSInt s = 0;
	if (ms < 0)
	{
		s = (ms / 1000) - 1;
		ms = ms - s * 1000;
	}
	else if (ms >= 1000)
	{
		s = ms / 1000;
		ms = ms % 1000;
	}
	tval->ms = (UInt16)ms;
	if (s != 0)
	{
		this->SetSecond(tval->second + s);
	}
}*/

void Data::DateTime::ClearTime()
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	tval->hour = 0;
	tval->minute = 0;
	tval->second = 0;
	this->ns = 0;
}

Int64 Data::DateTime::GetMSPassedDate()
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	return (Int64)(this->ns / 1000000) + tval->second * 1000 + tval->minute * 60000 + tval->hour * 3600000;
}

Int64 Data::DateTime::DiffMS(NN<DateTime> dt)
{
	return this->ToTicks() - dt->ToTicks();
}

Data::Duration Data::DateTime::Diff(NN<DateTime> dt)
{
	Int64 secs = this->ToUnixTimestamp() - dt->ToUnixTimestamp();
	UInt32 ns1 = this->ns;
	UInt32 ns2 = dt->ns;
	if (ns1 >= ns2)
	{
		return Data::Duration::FromSecNS(secs, ns1 - ns2);
	}
	else
	{
		return Data::Duration::FromSecNS(secs - 1, 1000000000 + ns1 - ns2);
	}
}

Bool Data::DateTime::IsYearLeap()
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	return Data::DateTimeUtil::IsYearLeap(tval->year);
}

Int64 Data::DateTime::ToTicks()
{
	if (this->timeType == TimeType::Ticks)
	{
		return this->val.secs * 1000LL + (this->ns / 1000000);
	}
	else if (this->timeType == TimeType::Time)
	{
		return Data::DateTimeUtil::TimeValue2Ticks(this->val.t, this->ns, this->tzQhr);
	}
	else
	{
		return 0;
	}
}

Int64 Data::DateTime::ToDotNetTicks()
{
	return ToTicks() * 10000LL + 621355968000000000LL;
}

Int64 Data::DateTime::ToUnixTimestamp()
{
	if (this->timeType == TimeType::Ticks)
	{
		return this->val.secs;
	}
	else if (this->timeType == TimeType::Time)
	{
		return Data::DateTimeUtil::TimeValue2Secs(this->val.t, this->tzQhr);
	}
	else
	{
		return 0;
	}
	return (this->ToTicks() / 1000LL);
}

Data::TimeInstant Data::DateTime::ToInstant()
{
	return Data::TimeInstant(this->ToUnixTimestamp(), this->ns);
}

void Data::DateTime::SetTicks(Int64 ticks)
{
	this->timeType = Data::DateTime::TimeType::Ticks;
	if (ticks < 0)
	{
		this->ns = (UInt32)(1000 - ticks % 1000) * 1000000;
		this->val.secs = ticks / 1000 - 1;
		if (this->ns >= 1000000000)
		{
			this->ns -= 1000000000;
			this->val.secs++;
		}
	}
	else
	{
		this->val.secs = ticks / 1000;
		this->ns = (UInt32)(ticks % 1000) * 1000000;
	}
}

void Data::DateTime::SetInstant(Data::TimeInstant instant)
{
	this->timeType = Data::DateTime::TimeType::Ticks;
	this->ns = instant.nanosec;
	this->val.secs = instant.sec;
}

void Data::DateTime::SetDotNetTicks(Int64 ticks)
{
	UInt32 nanosec;
	if (ticks < 0)
	{
		if (ticks % 10000000)
		{
			nanosec = (UInt32)((10000000 + (ticks % 10000000)) * 100);
			ticks = ticks / 10000000 - 1;
		}
		else
		{
			nanosec = 0;
			ticks = ticks / 10000000;
		}
	}
	else
	{
		nanosec = (UInt32)(ticks % 10000000) * 100;
		ticks = ticks / 10000000;
	}
	SetInstant(Data::TimeInstant(ticks, nanosec));
}

void Data::DateTime::SetUnixTimestamp(Int64 ticks)
{
	this->ns = 0;
	this->timeType = Data::DateTime::TimeType::Ticks;
	this->val.secs = ticks;
}

void Data::DateTime::SetMSDOSTime(UInt16 date, UInt16 time)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	tval->year = (UInt16)(1980 + ((date >> 9) & 0x7f));
	tval->month = (date >> 5) & 0xf;
	tval->day = date & 0x1f;
	tval->hour = (UInt8)(time >> 11);
	tval->minute = (time >> 5) & 0x3f;
	tval->second = (UInt8)((time & 0x1f) << 1);
	this->ns = 0;
}

UInt16 Data::DateTime::ToMSDOSDate()
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	return (UInt16)((((tval->year - 1980) & 0x7f) << 9) | (tval->month << 5) | tval->day);
}

UInt16 Data::DateTime::ToMSDOSTime()
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	return (UInt16)((tval->hour << 11) | (tval->minute << 5) | (tval->second >> 1));
}

void Data::DateTime::ToSYSTEMTIME(void *sysTime)
{
#if defined(WIN32) || defined(_WIN32_WCE)
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	SYSTEMTIME *stime = (SYSTEMTIME*)sysTime;
	stime->wYear = tval->year;
	stime->wMonth = tval->month;
	stime->wDay = tval->day;
	stime->wHour = tval->hour;
	stime->wMinute = tval->minute;
	stime->wSecond = tval->second;
	stime->wMilliseconds = (UInt16)(this->ns / 1000000);
	stime->wDayOfWeek = 0;
#endif
}

void Data::DateTime::SetNTPTime(Int32 hiDword, Int32 loDword)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	this->ToUTCTime();
	tval->year = 1900;
	tval->month = 1;
	tval->day = 1;
	tval->hour = 0;
	tval->minute = 0;
	tval->second = 0;
	this->ns = 0;
	this->AddMinute(hiDword / 60);
	this->AddMS((OSInt)((hiDword % 60) * 1000 + ((loDword * 1000LL) >> 32)));
}

Int64 Data::DateTime::ToNTPTime()
{
	Data::DateTime dt(1900, 1, 1, 0, 0, 0, 0);
	Int64 diff = this->DiffMS(dt);
	Int32 vals[2];
	vals[1] = (Int32)((diff / 1000LL) & 0xffffffffLL);
	vals[0] = (Int32)((0x100000000LL * (diff % 1000LL)) / 1000LL);
	return *(Int64*)vals;
}

UnsafeArray<Char> Data::DateTime::ToString(UnsafeArray<Char> buff)
{
	return UnsafeArray<Char>::ConvertFrom(ToString(UnsafeArray<UTF8Char>::ConvertFrom(buff)));
}

UnsafeArray<Char> Data::DateTime::ToString(UnsafeArray<Char> buff, UnsafeArray<const Char> pattern)
{
	return UnsafeArray<Char>::ConvertFrom(ToString(UnsafeArray<UTF8Char>::ConvertFrom(buff), pattern));
}

UnsafeArray<UTF8Char> Data::DateTime::ToString(UnsafeArray<UTF8Char> buff)
{
	if (this->ns == 0)
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss zzzz"));
	}
	else if (this->ns % 1000000 == 0)
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss.fff zzzz"));
	}
	else if (this->ns % 1000 == 0)
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss.ffffff zzzz"));
	}
	else
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss.fffffffff zzzz"));
	}
}

UnsafeArray<UTF8Char> Data::DateTime::ToStringNoZone(UnsafeArray<UTF8Char> buff)
{
	if (this->ns == 0)
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss"));
	}
	else if (this->ns % 1000000 == 0)
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss.fff"));
	}
	else if (this->ns % 1000 == 0)
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss.ffffff"));
	}
	else
	{
		return ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss.fffffffff"));
	}
}

UnsafeArray<UTF8Char> Data::DateTime::ToString(UnsafeArray<UTF8Char> buff, UnsafeArray<const Char> pattern)
{
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	return Data::DateTimeUtil::ToString(buff, tval, this->tzQhr, this->ns, UnsafeArray<const UTF8Char>::ConvertFrom(pattern));
}

Data::DateTime Data::DateTime::operator=(Data::DateTime dt)
{
	this->SetValue(dt);
	return *this;
}

Data::DateTime Data::DateTime::operator=(Int64 ticks)
{
	this->SetTicks(ticks);
	return *this;
}

Bool Data::DateTime::operator==(Data::DateTime dt)
{
	return ToTicks() == dt.ToTicks();
}

Bool Data::DateTime::operator>(Data::DateTime dt)
{
	return this->ToTicks() > dt.ToTicks();
}

Bool Data::DateTime::operator<(Data::DateTime dt)
{
	return this->ToTicks() < dt.ToTicks();
}

UnsafeArray<UTF8Char> Data::DateTime::ToLocalStr(UnsafeArray<UTF8Char> buff)
{
#if defined(WIN32) && !defined(_WIN32_WCE)
	tm t;
	NN<Data::DateTimeUtil::TimeValue> tval = this->GetTimeValue();
	t.tm_year = tval->year;
	t.tm_isdst = false;
	t.tm_mon = tval->month - 1;
	t.tm_mday = tval->day;
	t.tm_hour = tval->hour;
	t.tm_min = tval->minute;
	t.tm_sec = tval->second;
	return &buff[strftime((char*)buff.Ptr(), 100, "%c", &t)];
#else
	return this->ToString(buff, CHSTR("yyyy-MM-dd HH:mm:ss"));
#endif
}

OSInt Data::DateTime::CompareTo(NN<Data::DateTime> dt)
{
	Int64 ticks = this->ToTicks();
	Int64 ticks2 = dt->ToTicks();
	
	if (ticks > ticks2)
		return 1;
	else if (ticks < ticks2)
		return -1;
	else
		return 0;
}

Int32 Data::DateTime::DateCompare(NN<Data::DateTime> dt)
{
	NN<Data::DateTimeUtil::TimeValue> tval1 = this->GetTimeValue();
	NN<Data::DateTimeUtil::TimeValue> tval2 = dt->GetTimeValue();
	if (tval1->year < tval2->year)
		return -1;
	else if (tval1->year > tval2->year)
		return 1;
	else if (tval1->month < tval2->month)
		return -1;
	else if (tval1->month > tval2->month)
		return 1;
	else if (tval1->day < tval2->day)
		return -1;
	else if (tval1->day > tval2->day)
		return 1;
	else
		return 0;
}

Bool Data::DateTime::IsSameDay(NN<Data::DateTime> dt)
{
	NN<Data::DateTimeUtil::TimeValue> tval1 = this->GetTimeValue();
	NN<Data::DateTimeUtil::TimeValue> tval2 = dt->GetTimeValue();
	return tval1->year == tval2->year && tval1->month == tval2->month && tval1->day == tval2->day;
}

void Data::DateTime::ToUTCTime()
{
	if (this->tzQhr != 0)
	{
		Int32 tzQhr = this->tzQhr;
		this->tzQhr = 0;
		if (this->timeType == TimeType::Time)
		{
			this->AddMinute(tzQhr * -15);
		}
	}
}

void Data::DateTime::ToLocalTime()
{
	Int8 newTZ = Data::DateTimeUtil::GetLocalTzQhr();
	if (this->tzQhr != newTZ)
	{
		Int32 tzv = this->tzQhr - newTZ;
		this->tzQhr = newTZ;
		if (this->timeType == TimeType::Time)
		{
			this->AddMinute(tzv * -15);
		}
	}
}

void Data::DateTime::ConvertTimeZoneQHR(Int8 tzQhr)
{
	if (this->tzQhr != tzQhr)
	{
		Int32 tzv = this->tzQhr - tzQhr;
		this->tzQhr = tzQhr;
		if (this->timeType == TimeType::Time)
		{
			this->AddMinute(tzv * -15);
		}
	}
}

void Data::DateTime::SetTimeZoneQHR(Int8 tzQhr)
{
	if (this->timeType == TimeType::Ticks && this->tzQhr != tzQhr)
	{
		this->val.secs += (this->tzQhr - tzQhr) * (Int64)(15 * 60);
		this->tzQhr = tzQhr;
	}
	else
	{
		this->tzQhr = tzQhr;
	}
}

Int8 Data::DateTime::GetTimeZoneQHR()
{
	return this->tzQhr;
}

Data::DateTimeUtil::Weekday Data::DateTime::GetWeekday()
{
	return Data::DateTimeUtil::Ticks2Weekday(this->ToTicks(), this->tzQhr);
}

Data::Timestamp Data::DateTime::ToTimestamp()
{
	return Data::Timestamp(Data::TimeInstant(this->ToUnixTimestamp(), this->ns), this->tzQhr);
}
