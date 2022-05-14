#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Math/Math.h"
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

Data::DateTimeUtil::TimeValue *Data::DateTime::GetTimeValue()
{
	Data::DateTimeUtil::TimeValue *t = &this->val.t;
	switch (this->timeType)
	{
	case TimeType::Time:
		return t;
	case TimeType::Ticks:
		this->timeType = TimeType::Time;
		Data::DateTimeUtil::Ticks2TimeValue(this->val.ticks, t, this->tzQhr);
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
		t->ms = 0;
		return t;		
	}
}

void Data::DateTime::SetDate(Data::DateTimeUtil::TimeValue *t, Text::PString *dateStrs)
{
	UInt32 vals[3];
	vals[0] = 0;
	vals[1] = 0;
	vals[2] = 0;
	Text::StrToUInt32(dateStrs[0].v, &vals[0]);
	Text::StrToUInt32(dateStrs[1].v, &vals[1]);
	Text::StrToUInt32(dateStrs[2].v, &vals[2]);
	if (vals[0] > 100)
	{
		t->year = (UInt16)vals[0];
		t->month = (UInt8)vals[1];
		t->day = (UInt8)vals[2];
	}
	else if (vals[2] > 100)
	{
		t->year = (UInt16)vals[2];
		if (vals[0] > 12)
		{
			t->month = (UInt8)vals[1];
			t->day = (UInt8)vals[0];
		}
		else
		{
			t->month = (UInt8)vals[0];
			t->day = (UInt8)vals[1];
		}
	}
	else
	{
		if (vals[1] > 12)
		{
			t->year = (UInt16)(((t->year / 100) * 100U) + vals[2]);
			t->month = (UInt8)vals[0];
			t->day = (UInt8)vals[1];
		}
		else
		{
			t->year = (UInt16)(((t->year / 100) * 100U) + vals[0]);
			t->month = (UInt8)vals[1];
			t->day = (UInt8)vals[2];
		}
	}
}

void Data::DateTime::SetTime(Data::DateTimeUtil::TimeValue *t, Text::PString *timeStrs)
{
	Text::PString strs[2];
	UOSInt valTmp;

	t->hour = (UInt8)Text::StrToUInt32(timeStrs[0].v);
	t->minute = (UInt8)Text::StrToUInt32(timeStrs[1].v);
	valTmp = Text::StrSplitP(strs, 2, timeStrs[2], '.');
	if (valTmp == 1)
	{
		t->second = (UInt8)Text::StrToUInt32(strs[0].v);
		t->ms = 0;
	}
	else
	{
		t->second = (UInt8)Text::StrToUInt32(strs[0].v);
		valTmp = strs[1].leng;
		if (valTmp == 1)
		{
			t->ms = (UInt16)(Text::StrToUInt32(strs[1].v) * 100);
		}
		else if (valTmp == 2)
		{
			t->ms = (UInt16)(Text::StrToUInt32(strs[1].v) * 10);
		}
		else if (valTmp == 3)
		{
			t->ms = (UInt16)Text::StrToUInt32(strs[1].v);
		}
		else if (valTmp > 3)
		{
			strs[1].v[3] = 0;
			t->ms = (UInt16)Text::StrToUInt32(strs[1].v);
		}
		else
		{
			t->ms = 0;
		}
	}
}

void Data::DateTime::FixValues()
{
	Data::DateTimeUtil::TimeValue *t = GetTimeValue();
	while (t->ms >= 1000)
	{
		t->ms = (UInt16)(t->ms - 1000);
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
	this->tzQhr = 0;
}

Data::DateTime::DateTime(Int64 ticks)
{
	this->timeType = TimeType::Ticks;
	this->val.ticks = ticks;
	this->tzQhr = 0;
}

Data::DateTime::DateTime(Int64 ticks, Int8 tzQhr)
{
	this->timeType = TimeType::Ticks;
	this->val.ticks = ticks;
	this->tzQhr = tzQhr;
}

Data::DateTime::DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second)
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
	t->ms = 0;
}

Data::DateTime::DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms)
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
	t->ms = ms;
}

Data::DateTime::DateTime(Text::CString dateStr)
{
	this->timeType = TimeType::None;
	this->tzQhr = 0;
	this->SetValue(dateStr);
}

Data::DateTime::DateTime(Data::DateTime *dt)
{
	this->SetValue(dt);
}

Data::DateTime::~DateTime()
{
}

Bool Data::DateTime::SetAsComputerTime()
{
#ifdef WIN32
	Data::DateTimeUtil::TimeValue *t = GetTimeValue();
	SYSTEMTIME st;
	ToUTCTime();
	st.wYear = t->year;
	st.wMonth = t->month;
	st.wDay = t->day;
	st.wHour = t->hour;
	st.wMinute = t->minute;
	st.wSecond = t->second;
	st.wMilliseconds = t->ms;
	return SetSystemTime(&st) != FALSE;
#elif !defined(CPU_AVR)
	struct timespec tp;
	tp.tv_sec = this->ToTicks() / 1000;
	tp.tv_nsec = 0;
	clock_settime(CLOCK_REALTIME, &tp);
	return clock_settime(CLOCK_REALTIME, &tp) == 0;
#else
	return false;
#endif
}

Data::DateTime *Data::DateTime::SetCurrTime()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef WIN32
	Data::DateTimeUtil::TimeValue *t = GetTimeValue();
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
	t->ms = st.wMilliseconds;
	this->tzQhr = (Int8)(-tz.Bias / 15);
#endif
	return this;
#elif !defined(CPU_AVR)
	struct timeval tv;
	if (gettimeofday(&tv, 0) == 0)
	{
		this->SetTicks(1000 * (Int64)tv.tv_sec + tv.tv_usec / 1000);
	}
	else
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		this->SetTicks(1000 * (Int64)ts.tv_sec + (ts.tv_nsec / 1000000));
	}
	this->ToLocalTime();
	return this;
#else
	return this;
#endif
}

Data::DateTime *Data::DateTime::SetCurrTimeUTC()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef WIN32
	SYSTEMTIME st;
	GetSystemTime(&st);
	Data::DateTimeUtil::TimeValue *tval = GetTimeValue();
	tval->year = st.wYear;
	tval->month = (UInt8)st.wMonth;
	tval->day = (UInt8)st.wDay;
	tval->hour = (UInt8)st.wHour;
	tval->minute = (UInt8)st.wMinute;
	tval->second = (UInt8)st.wSecond;
	tval->ms = st.wMilliseconds;
	this->tzQhr = 0;
#endif
	return this;
#elif !defined(CPU_AVR)
	this->tzQhr = 0;
	struct timeval tv;
	if (gettimeofday(&tv, 0) == 0)
	{
		this->SetTicks(1000 * (Int64)tv.tv_sec + tv.tv_usec / 1000);
	}
	else
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		this->SetTicks(1000 * (Int64)ts.tv_sec + (ts.tv_nsec / 1000000));
	}
	return this;
#else
	return this;
#endif
}

void Data::DateTime::SetValue(const DateTime *time)
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
		this->val.t.ms = time->val.t.ms;
	}
	else
	{
		this->val.ticks = time->val.ticks;
	}
}

void Data::DateTime::SetValue(UInt16 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms, Int8 tzQhr)
{
	this->SetDate(year, month, day);
	this->SetHour(hour);
	this->SetMinute(minute);
	this->SetSecond(second);
	this->SetMS(ms);
	this->tzQhr = tzQhr;
}

void Data::DateTime::SetValue(UInt16 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms)
{
	this->SetDate(year, month, day);
	this->SetHour(hour);
	this->SetMinute(minute);
	this->SetSecond(second);
	this->SetMS(ms);
}

void Data::DateTime::SetValue(Int64 ticks, Int8 tzQhr)
{
	this->timeType = Data::DateTime::TimeType::Ticks;
	this->val.ticks = ticks;
	this->tzQhr = tzQhr;
}

void Data::DateTime::SetValueNoFix(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms, Int8 tzQhr)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	tval->year = year;
	tval->month = month;
	tval->day = day;
	tval->hour = hour;
	tval->minute = minute;
	tval->second = second;
	tval->ms = ms;
	this->tzQhr = tzQhr;	
}

Bool Data::DateTime::SetValueSlow(const Char *dateStr)
{
	return this->SetValue(Text::CString((const UTF8Char*)dateStr, Text::StrCharCnt(dateStr)));
}

Bool Data::DateTime::SetValue(Text::CString dateStr)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	UTF8Char buff[32];
	Text::PString strs2[5];
	Text::PString strs[3];
	UOSInt nStrs;
	Bool succ = true;
	if (dateStr.v[3] == ',' && Text::StrIndexOfChar(&dateStr.v[4], ',') == INVALID_INDEX)
	{
		dateStr = dateStr.Substring(4);
		while (dateStr.v[0] == ' ')
			dateStr = dateStr.Substring(1);
	}
	dateStr.ConcatTo(buff);
	nStrs = Text::StrSplitTrimP(strs2, 5, {buff, dateStr.leng}, ' ');
	if (nStrs == 1)
	{
		nStrs = Text::StrSplitP(strs2, 3, {buff, dateStr.leng}, 'T');
	}
	if (nStrs == 2)
	{
		Bool dateSucc = true;
		if (Text::StrSplitP(strs, 3, strs2[0], '-') == 3)
		{
			SetDate(tval, strs);
		}
		else if (Text::StrSplitP(strs, 3, strs2[0], '/') == 3)
		{
			SetDate(tval, strs);
		}
		else if (Text::StrSplitP(strs, 3, strs2[0], ':') == 3)
		{
			SetDate(tval, strs);
		}
		else
		{
			SetCurrTime();
			dateSucc = false;
		}
		UOSInt i = strs2[1].IndexOf('-');
		if (i == INVALID_INDEX)
		{
			i = strs2[1].IndexOf('+');
		}
		if (i != INVALID_INDEX)
		{
			UTF8Char c = strs2[1].v[i];
			strs2[1].v[i] = 0;
			UOSInt tzlen = strs2[1].leng - i - 1;
			if (tzlen == 5)
			{
				UInt32 min = Text::StrToUInt32(&strs2[1].v[i + 4]);
				if (strs2[1].v[i + 3] == ':')
				{
					strs2[1].v[i + 3] = 0;
				}
				else
				{
					strs2[1].v[i + 4] = 0;
				}
				min = min + Text::StrToUInt32(&strs2[1].v[i + 1]) * 60;
				if (c == '-')
				{
					this->tzQhr = (Int8)(-(Int32)min / 15);
				}
				else
				{
					this->tzQhr = (Int8)(min / 15);
				}
			}
			else if (tzlen == 2)
			{
				if (c == '-')
				{
					this->tzQhr = (Int8)-(Text::StrToInt32(&strs2[1].v[i + 1]) * 4);
				}
				else
				{
					this->tzQhr = (Int8)(Text::StrToUInt32(&strs2[1].v[i + 1]) * 4);
				}
			}
			strs2[1].leng = i;
		}
		if (Text::StrSplitP(strs, 3, strs2[1], ':') == 3)
		{
			if (strs[2].v[strs[2].leng - 1] == 'Z')
			{
				strs[2].v[strs[2].leng - 1] = 0;
				strs[2].leng -= 1;
			}
			SetTime(tval, strs);
		}
		else
		{
			tval->hour = 0;
			tval->minute = 0;
			tval->second = 0;
			tval->ms = 0;
			if (!dateSucc)
			{
				succ = false;
			}
		}
	}
	else if (nStrs == 1)
	{
		if (Text::StrSplitP(strs, 3, strs2[0], '-') == 3)
		{
			SetDate(tval, strs);
			tval->hour = 0;
			tval->minute = 0;
			tval->second = 0;
			tval->ms = 0;
		}
		else if (Text::StrSplitP(strs, 3, strs2[0], '/') == 3)
		{
			SetDate(tval, strs);
			tval->hour = 0;
			tval->minute = 0;
			tval->second = 0;
			tval->ms = 0;
		}
		else if (Text::StrSplitP(strs, 3, strs2[0], ':') == 3)
		{
			SetCurrTime();
			SetTime(tval, strs);
		}
		else
		{
			succ = false;
		}
	}
	else if (nStrs == 4 || (nStrs == 5 && (strs2[4].v[0] == '-' || strs2[4].v[0] == '+' || strs2[4].Equals(UTF8STRC("GMT")))))
	{
		UOSInt len1 = strs2[0].leng;
		UOSInt len2 = strs2[1].leng;
		UOSInt len3 = strs2[2].leng;
		UOSInt len4 = strs2[3].leng;
		UTF8Char *timeStr = strs2[3].v;
		UOSInt timeStrLen = strs2[3].leng;
		if (len1 == 3 && len2 <= 2 && len3 == 4)
		{
			Text::StrToUInt16(strs2[2].v, &tval->year);
			tval->month = Data::DateTimeUtil::ParseMonthStr(strs2[0].v, strs2[0].leng);
			tval->day = Text::StrToUInt8(strs2[1].v);
		}
		else if (len1 <= 2 && len2 == 3 && len3 == 4)
		{
			Text::StrToUInt16(strs2[2].v, &tval->year);
			tval->month = Data::DateTimeUtil::ParseMonthStr(strs2[1].v, strs2[1].leng);
			tval->day = Text::StrToUInt8(strs2[0].v);
		}
		else if (len1 == 3 && len2 <= 2 && len4 == 4)
		{
			Text::StrToUInt16(strs2[3].v, &tval->year);
			tval->month = Data::DateTimeUtil::ParseMonthStr(strs2[0].v, strs2[0].leng);
			tval->day = Text::StrToUInt8(strs2[1].v);
			timeStr = strs2[2].v;
			timeStrLen = strs2[2].leng;
		}
		else
		{
			succ = false;
		}
		if (Text::StrSplitP(strs, 3, {timeStr, timeStrLen}, ':') == 3)
		{
			SetTime(tval, strs);
		}
		else
		{
			succ = false;
		}
		
		if (nStrs == 5)
		{
			if (strs2[4].Equals(UTF8STRC("GMT")))
			{
				this->tzQhr = 0;
			}
			else if (strs2[4].leng == 5)
			{
				Int32 min = (Int32)Text::StrToUInt32(&strs2[4].v[3]);
				if (strs2[4].v[2] == ':')
				{
					strs2[4].v[2] = 0;
				}
				else
				{
					strs2[4].v[3] = 0;
				}
				min = min + Text::StrToInt32(&strs2[4].v[1]) * 60;
				if (strs2[4].v[0] == '-')
				{
					this->tzQhr = (Int8)(-min / 15);
				}
				else
				{
					this->tzQhr = (Int8)(min / 15);
				}
			}
		}
	}
	else
	{
		tval->year = 1970;
		tval->month = 1;
		tval->day = 1;
		tval->hour = 0;
		tval->minute = 0;
		tval->second = 0;
		tval->ms = 0;
		this->tzQhr = 0;

		UOSInt j = 0;
		UOSInt i;
		UOSInt splitCnt;
		while (true)
		{
			if ((splitCnt = Text::StrSplitP(strs, 3, strs2[j], ':')) == 3)
			{
				SetTime(tval, strs);
			}
			else
			{
				if (splitCnt == 2)
				{
					strs[1].v[-1] = ':';
				}
				if (strs2[j].v[0] == '-')
				{
					if (Text::StrSplitP(strs, 3, strs2[j], ':') == 2)
					{
						this->tzQhr = (Int8)(-(Int32)((Text::StrToUInt32(&strs[0].v[1]) << 2) + (Text::StrToUInt32(strs[1].v) / 15)));
					}
					else if (strs2[j].leng == 5)
					{
						this->tzQhr = (Int8)(Text::StrToUInt32(&strs2[j].v[3]) / 15);
						strs2[j].v[3] = 0;
						this->tzQhr = (Int8)(-(this->tzQhr + (Int32)(Text::StrToUInt32(&strs2[j].v[1]) << 2)));
					}
				}
				else if (strs2[j].v[0] == '+')
				{
					if (Text::StrSplitP(strs, 3, strs2[j], ':') == 2)
					{
						this->tzQhr = (Int8)((Text::StrToUInt32(&strs[0].v[1]) << 2) + (Text::StrToUInt32(strs[1].v) / 15));
					}
					else if (strs2[j].leng == 5)
					{
						this->tzQhr = (Int8)(Text::StrToUInt32(&strs2[j].v[3]) / 15);
						strs2[j].v[3] = 0;
						this->tzQhr = (Int8)(this->tzQhr + (Int32)(Text::StrToUInt32(&strs2[j].v[1]) << 2));
					}
				}
				else
				{
					i = Text::StrIndexOfChar(strs2[j].v, '/');
					if (i != INVALID_INDEX && i > 0)
					{
						if (Text::StrSplitP(strs, 3, strs2[0], '/') == 3)
						{
							SetDate(tval, strs);
						}
					}
					else if (Text::StrSplitP(strs, 3, strs2[0], '-') == 3)
					{
						SetDate(tval, strs);
					}
					else if (strs2[j].Equals(UTF8STRC("HKT")))
					{
						this->tzQhr = 32;
					}
					else
					{
						i = Text::StrToUInt32(strs2[j].v);
						if (i <= 0)
						{
							i = Data::DateTimeUtil::ParseMonthStr(strs2[j].v, strs2[j].leng);
							if (i > 0)
							{
								tval->month = (UInt8)i;
							}
						}
						else if (i > 100)
						{
							tval->year = (UInt16)i;
						}
						else
						{
							tval->day = (UInt8)i;
						}
					}
				}
			}
			if (j == 0)
			{
				j = 1;
			}
			else if (nStrs == 1)
			{
				break;
			}
			else
			{
				nStrs = Text::StrSplitP(&strs2[1], 2, strs2[2], ' ');
			}
		}
	}
	return succ;
}

void Data::DateTime::SetValueSYSTEMTIME(void *sysTime)
{
	SYSTEMTIME *stime = (SYSTEMTIME*)sysTime;
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	tval->year = stime->wYear;
	tval->month = (UInt8)stime->wMonth;
	tval->day = (UInt8)stime->wDay;
	tval->hour = (UInt8)stime->wHour;
	tval->minute = (UInt8)stime->wMinute;
	tval->second = (UInt8)stime->wSecond;
	tval->ms = stime->wMilliseconds;
	this->tzQhr = 0;
}

void Data::DateTime::SetValueFILETIME(void *fileTime)
{
#if defined(_WIN32)
	FILETIME *ftime = (FILETIME*)fileTime;
	SYSTEMTIME stime;
	FileTimeToSystemTime(ftime, &stime);
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	tval->year = stime.wYear;
	tval->month = (UInt8)stime.wMonth;
	tval->day = (UInt8)stime.wDay;
	tval->hour = (UInt8)stime.wHour;
	tval->minute = (UInt8)stime.wMinute;
	tval->second = (UInt8)stime.wSecond;
	tval->ms = stime.wMilliseconds;
	this->tzQhr = 0;
#else
	this->SetTicks(ReadInt64((const UInt8*)fileTime) / 10000 - 11644473600000LL);
#endif
}

void Data::DateTime::SetValueVariTime(Double variTime)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	tval->year = 1900;
	tval->month = 1;
	tval->day = 1;
	tval->hour = 0;
	tval->minute = 0;
	tval->second = 0;
	tval->ms = 0;
	this->tzQhr = 0;
	OSInt d = (OSInt)variTime;
	this->AddDay(d);
	variTime = (variTime - OSInt2Double(d)) * 24;
	tval->hour = (UInt8)variTime;
	variTime = (variTime - tval->hour) * 60;
	tval->minute = (UInt8)variTime;
	variTime = (variTime - tval->minute) * 60;
	tval->second = (UInt8)variTime;
	variTime = (variTime - tval->second) * 1000;
	tval->ms = (UInt16)variTime;
}

UInt16 Data::DateTime::GetYear()
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
	return this->GetTimeValue()->ms;
}

Data::DateTime *Data::DateTime::AddMonth(OSInt val)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
	return this;
}

Data::DateTime *Data::DateTime::AddDay(OSInt val)
{
	switch (this->timeType)
	{
	case TimeType::Time:
		{
			Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
		this->val.ticks = val * 86400000LL;
		break;
	case TimeType::Ticks:
		this->val.ticks += val * 86400000LL;
		break;
	}
	return this;
}

Data::DateTime *Data::DateTime::AddHour(OSInt val)
{
	switch (this->timeType)
	{
	case TimeType::Time:
		{
			Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
		this->val.ticks += val * 3600000LL;
		break;
	case TimeType::None:
	default:
		this->val.ticks = val * 3600000LL;
		this->timeType = TimeType::Ticks;
		break;
	}
	return this;
}

Data::DateTime *Data::DateTime::AddMinute(OSInt val)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
	return this;
}

Data::DateTime *Data::DateTime::AddSecond(OSInt val)
{
	if (this->timeType == TimeType::Time)
	{
		Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
		return this;
	}
	else
	{
		this->val.ticks += (Int64)val * 1000;
		return this;
	}
}

Data::DateTime *Data::DateTime::AddMS(OSInt val)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	OSInt seconds = val / 1000;
	OSInt outMS;
	outMS = val - seconds * 1000 + tval->ms;
	while (outMS < 0)
	{
		outMS += 1000;
		seconds--;
	}
	while (outMS >= 1000)
	{
		outMS -= 1000;
		seconds++;
	}
	tval->ms = (UInt16)outMS;
	if (seconds)
		AddSecond(seconds);
	return this;
}

void Data::DateTime::SetDate(UInt16 year, OSInt month, OSInt day)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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

void Data::DateTime::SetYear(UInt16 year)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	tval->year = year;
}

void Data::DateTime::SetMonth(OSInt month)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	this->SetDate(tval->year, month, tval->day);
}

void Data::DateTime::SetDay(OSInt day)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	this->SetDate(tval->year, tval->month, day);
}

void Data::DateTime::SetHour(OSInt hour)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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

void Data::DateTime::SetMS(OSInt ms)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
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
}

void Data::DateTime::ClearTime()
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	tval->hour = 0;
	tval->minute = 0;
	tval->second = 0;
	tval->ms = 0;
}

Int64 Data::DateTime::GetMSPassedDate()
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	return tval->ms + tval->second * 1000 + tval->minute * 60000 + tval->hour * 3600000;
}

Int64 Data::DateTime::DiffMS(DateTime *dt)
{
	return this->ToTicks() - dt->ToTicks();
}

Bool Data::DateTime::IsYearLeap()
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	return Data::DateTimeUtil::IsYearLeap(tval->year);
}

Int64 Data::DateTime::ToTicks()
{
	if (this->timeType == TimeType::Ticks)
	{
		return this->val.ticks;
	}
	else if (this->timeType == TimeType::Time)
	{
		return Data::DateTimeUtil::TimeValue2Ticks(&this->val.t, this->tzQhr);
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
	return (this->ToTicks() / 1000LL);
}

void Data::DateTime::SetTicks(Int64 ticks)
{
	this->timeType = TimeType::Ticks;
	this->val.ticks = ticks;
}

void Data::DateTime::SetDotNetTicks(Int64 ticks)
{
	SetTicks(ticks / 10000LL - 62135596800000LL);
}

void Data::DateTime::SetUnixTimestamp(Int64 ticks)
{
	SetTicks(ticks * 1000LL);
}

void Data::DateTime::SetMSDOSTime(UInt16 date, UInt16 time)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	tval->year = (UInt16)(1980 + ((date >> 9) & 0x7f));
	tval->month = (date >> 5) & 0xf;
	tval->day = date & 0x1f;
	tval->hour = (UInt8)(time >> 11);
	tval->minute = (time >> 5) & 0x3f;
	tval->second = (time & 0x1f) << 1;
	tval->ms = 0;
}

UInt16 Data::DateTime::ToMSDOSDate()
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	return (UInt16)((((tval->year - 1980) & 0x7f) << 9) | (tval->month << 5) | tval->day);
}

UInt16 Data::DateTime::ToMSDOSTime()
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	return (UInt16)((tval->hour << 11) | (tval->minute << 5) | (tval->second >> 1));
}

void Data::DateTime::ToSYSTEMTIME(void *sysTime)
{
#if defined(WIN32) || defined(_WIN32_WCE)
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	SYSTEMTIME *stime = (SYSTEMTIME*)sysTime;
	stime->wYear = tval->year;
	stime->wMonth = tval->month;
	stime->wDay = tval->day;
	stime->wHour = tval->hour;
	stime->wMinute = tval->minute;
	stime->wSecond = tval->second;
	stime->wMilliseconds = tval->ms;
	stime->wDayOfWeek = 0;
#endif
}

void Data::DateTime::SetNTPTime(Int32 hiDword, Int32 loDword)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	this->ToUTCTime();
	tval->year = 1900;
	tval->month = 1;
	tval->day = 1;
	tval->hour = 0;
	tval->minute = 0;
	tval->second = 0;
	tval->ms = 0;
	this->AddMinute(hiDword / 60);
	this->AddMS((hiDword % 60) * 1000 + ((loDword * 1000LL) >> 32));
}

Int64 Data::DateTime::ToNTPTime()
{
	Data::DateTime dt(1900, 1, 1, 0, 0, 0, 0);
	Int64 diff = this->DiffMS(&dt);
	Int32 vals[2];
	vals[1] = (Int32)((diff / 1000LL) & 0xffffffffLL);
	vals[0] = (Int32)((0x100000000LL * (diff % 1000LL)) / 1000LL);
	return *(Int64*)vals;
}

Char *Data::DateTime::ToString(Char *buff)
{
	return (Char*)ToString((UTF8Char*)buff);
}

Char *Data::DateTime::ToString(Char *buff, const Char *pattern)
{
	return (Char*)ToString((UTF8Char*)buff, pattern);
}

UTF8Char *Data::DateTime::ToString(UTF8Char *buff)
{
	return ToString(buff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
}

UTF8Char *Data::DateTime::ToString(UTF8Char *buff, const Char *pattern)
{
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	return Data::DateTimeUtil::ToString(buff, tval, this->tzQhr, (const UTF8Char*)pattern);
}

Data::DateTime Data::DateTime::operator=(Data::DateTime dt)
{
	this->SetValue(&dt);
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

UTF8Char *Data::DateTime::ToLocalStr(UTF8Char *buff)
{
#if defined(WIN32) && !defined(_WIN32_WCE)
	tm t;
	Data::DateTimeUtil::TimeValue *tval = this->GetTimeValue();
	t.tm_year = tval->year;
	t.tm_isdst = false;
	t.tm_mon = tval->month - 1;
	t.tm_mday = tval->day;
	t.tm_hour = tval->hour;
	t.tm_min = tval->minute;
	t.tm_sec = tval->second;
	return &buff[strftime((char*)buff, 100, "%c", &t)];
#else
	return this->ToString(buff, "yyyy-MM-dd HH:mm:ss");
#endif
}

OSInt Data::DateTime::CompareTo(Data::DateTime *dt)
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

Int32 Data::DateTime::DateCompare(Data::DateTime *dt)
{
	Data::DateTimeUtil::TimeValue *tval1 = this->GetTimeValue();
	Data::DateTimeUtil::TimeValue *tval2 = dt->GetTimeValue();
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

Bool Data::DateTime::IsSameDay(Data::DateTime *dt)
{
	Data::DateTimeUtil::TimeValue *tval1 = this->GetTimeValue();
	Data::DateTimeUtil::TimeValue *tval2 = dt->GetTimeValue();
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
		this->val.ticks += (tzQhr - this->tzQhr) * (Int64)(15 * 60000);
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

