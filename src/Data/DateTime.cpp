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
#endif
#include <time.h>
#include <stdio.h>

void Data::DateTime::SetDate(Char **dateStrs)
{
	UInt32 vals[3];
	vals[0] = 0;
	vals[1] = 0;
	vals[2] = 0;
	Text::StrToUInt32(dateStrs[0], &vals[0]);
	Text::StrToUInt32(dateStrs[1], &vals[1]);
	Text::StrToUInt32(dateStrs[2], &vals[2]);
	if (vals[0] > 100)
	{
		this->year = (UInt16)vals[0];
		this->month = (UInt8)vals[1];
		this->day = (UInt8)vals[2];
	}
	else if (vals[2] > 100)
	{
		this->year = (UInt16)vals[2];
		if (vals[0] > 12)
		{
			this->month = (UInt8)vals[1];
			this->day = (UInt8)vals[0];
		}
		else
		{
			this->month = (UInt8)vals[0];
			this->day = (UInt8)vals[1];
		}
	}
	else
	{
		if (vals[1] > 12)
		{
			this->year = (UInt16)(((this->year / 100) * 100U) + vals[2]);
			this->month = (UInt8)vals[0];
			this->day = (UInt8)vals[1];
		}
		else
		{
			this->year = (UInt16)(((this->year / 100) * 100U) + vals[0]);
			this->month = (UInt8)vals[1];
			this->day = (UInt8)vals[2];
		}
	}
}

void Data::DateTime::SetTime(Char **timeStrs)
{
	Char *strs[2];
	UOSInt valTmp;

	this->hour = (UInt8)Text::StrToInt32(timeStrs[0]);
	this->minute = (UInt8)Text::StrToInt32(timeStrs[1]);
	valTmp = Text::StrSplit(strs, 2, timeStrs[2], '.');
	if (valTmp == 1)
	{
		this->second = (UInt8)Text::StrToInt32(strs[0]);
		this->ms = 0;
	}
	else
	{
		this->second = (UInt8)Text::StrToInt32(strs[0]);
		valTmp = Text::StrCharCnt(strs[1]);
		if (valTmp == 1)
		{
			this->ms = (UInt16)(Text::StrToInt32(strs[1]) * 100);
		}
		else if (valTmp == 2)
		{
			this->ms = (UInt16)(Text::StrToInt32(strs[1]) * 10);
		}
		else if (valTmp == 3)
		{
			this->ms = (UInt16)Text::StrToInt32(strs[1]);
		}
		else
		{
			this->ms = 0;
		}
	}
}

void Data::DateTime::FixValues()
{
	while (this->ms >= 1000)
	{
		this->ms = (UInt16)(this->ms - 1000);
		this->second++;
	}
	while (this->second >= 60)
	{
		this->second = (UInt8)(this->second - 60);
		this->minute++;
	}
	while (this->minute >= 60)
	{
		this->minute = (UInt8)(this->minute - 60);
		this->hour++;
	}
	while (this->hour >= 24)
	{
		this->hour = (UInt8)(this->hour - 24);
		this->day++;
	}
	while (this->day < 1)
	{
		this->month--;
		this->day = (UInt8)(this->day + this->DayInMonth(this->year, this->month));
	}
	UInt32 i;
	while (this->day > (i = this->DayInMonth(this->year, this->month)))
	{
		this->day = (UInt8)(this->day - i);
		this->month++;
	}
	while (this->month < 1)
	{
		this->month = (UInt8)(this->month + 12);
		this->year--;
	}
	while (this->month > 12)
	{
		this->month = (UInt8)(this->month - 12);
		this->year++;
	}
}

Data::DateTime::DateTime()
{
	this->year = 1970;
	this->month = 1;
	this->day = 1;
	this->hour = 0;
	this->minute = 0;
	this->second = 0;
	this->ms = 0;
	this->tzQhr = 0;
}

Data::DateTime::DateTime(Int64 ticks)
{
	this->tzQhr = 0;
	(*this) = ticks;
}

Data::DateTime::DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second)
{
	this->year = year;
	this->month = month;
	this->day = day;
	this->hour = hour;
	this->minute = minute;
	this->second = second;
	this->ms = 0;
	this->tzQhr = 0;
}

Data::DateTime::DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms)
{
	this->year = year;
	this->month = month;
	this->day = day;
	this->hour = hour;
	this->minute = minute;
	this->second = second;
	this->ms = ms;
	this->tzQhr = 0;
}

Data::DateTime::DateTime(const UTF8Char *dateStr)
{
	SetValue(dateStr);
}

Data::DateTime::DateTime(Data::DateTime *dt)
{
	this->year = dt->year;
	this->month = dt->month;
	this->day = dt->day;
	this->hour = dt->hour;
	this->minute = dt->minute;
	this->second = dt->second;
	this->ms = dt->ms;
	this->tzQhr = dt->tzQhr;
}

Data::DateTime::~DateTime()
{
}

Bool Data::DateTime::SetAsComputerTime()
{
#ifdef WIN32
	SYSTEMTIME st;
	ToUTCTime();
	st.wYear = this->year;
	st.wMonth = this->month;
	st.wDay = this->day;
	st.wHour = this->hour;
	st.wMinute = this->minute;
	st.wSecond = this->second;
	st.wMilliseconds = this->ms;
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
	SYSTEMTIME st;
	TIME_ZONE_INFORMATION tz;
	GetLocalTime(&st);
	tz.Bias = 0;
	GetTimeZoneInformation(&tz);
	this->year = st.wYear;
	this->month = (UInt8)st.wMonth;
	this->day = (UInt8)st.wDay;
	this->hour = (UInt8)st.wHour;
	this->minute = (UInt8)st.wMinute;
	this->second = (UInt8)st.wSecond;
	this->ms = st.wMilliseconds;
	this->tzQhr = (Int8)(-tz.Bias / 15);
#endif
	return this;
#elif defined(__sun__)
	time_t now = time(0);
	tm *t = localtime(&now);
	Int32 newTZ = mktime(t) - now;
	if (t->tm_isdst > 0)
	{
    	newTZ = newTZ - 60 * 60;
	}
	printf("timezone = %d\r\n", newTZ);
	this->year = t->tm_year + 1900;
	this->month = (UInt8)t->tm_mon + 1;
	this->day = (UInt8)t->tm_mday;
	this->hour = (UInt8)t->tm_hour;
	this->minute = (UInt8)t->tm_min;
	this->second = (UInt8)t->tm_sec;
	this->ms = 0;
	this->tzQhr = newTZ / 900;

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	this->SetUnixTimestamp(ts.tv_sec);
	this->ms = ts.tv_nsec / 1000000;
	return this;
#elif !defined(CPU_AVR)
	time_t now = time(0);
	tm *t = localtime(&now);
	Int32 newTZ = (Int32)(t->tm_gmtoff / 900);
	this->year = (UInt16)(t->tm_year + 1900);
	this->month = (UInt8)(t->tm_mon + 1);
	this->day = (UInt8)t->tm_mday;
	this->hour = (UInt8)t->tm_hour;
	this->minute = (UInt8)t->tm_min;
	this->second = (UInt8)t->tm_sec;
	this->ms = 0;
	this->tzQhr = (Int8)newTZ;

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	this->SetUnixTimestamp(ts.tv_sec);
	this->ms = (UInt16)(ts.tv_nsec / 1000000);
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
	this->year = st.wYear;
	this->month = (UInt8)st.wMonth;
	this->day = (UInt8)st.wDay;
	this->hour = (UInt8)st.wHour;
	this->minute = (UInt8)st.wMinute;
	this->second = (UInt8)st.wSecond;
	this->ms = st.wMilliseconds;
	this->tzQhr = 0;
#endif
	return this;
#elif !defined(CPU_AVR)
	this->tzQhr = 0;
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	this->SetUnixTimestamp(ts.tv_sec);
	this->ms = (UInt16)(ts.tv_nsec / 1000000);
	return this;
#else
	return this;
#endif
}

void Data::DateTime::SetValue(const DateTime *time)
{
	this->year = time->year;
	this->month = time->month;
	this->day = time->day;
	this->hour = time->hour;
	this->minute = time->minute;
	this->second = time->second;
	this->ms = time->ms;
	this->tzQhr = time->tzQhr;
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

Bool Data::DateTime::SetValue(const Char *dateStr)
{
	Char buff[32];
	Char *strs2[5];
	Char *strs[3];
	UOSInt nStrs;
	Bool succ = true;
	if (dateStr[3] == ',' && Text::StrIndexOf(&dateStr[4], ',') == INVALID_INDEX)
	{
		dateStr += 4;
		while (*dateStr == ' ')
			dateStr++;
	}
	Text::StrConcat(buff, dateStr);
	nStrs = Text::StrSplitTrim(strs2, 5, buff, ' ');
	if (nStrs == 1)
	{
		nStrs = Text::StrSplit(strs2, 3, buff, 'T');
	}
	if (nStrs == 2)
	{
		Bool dateSucc = true;
		if (Text::StrSplit(strs, 3, strs2[0], '-') == 3)
		{
			SetDate(strs);
		}
		else if (Text::StrSplit(strs, 3, strs2[0], '/') == 3)
		{
			SetDate(strs);
		}
		else if (Text::StrSplit(strs, 3, strs2[0], ':') == 3)
		{
			SetDate(strs);
		}
		else
		{
			SetCurrTime();
			dateSucc = false;
		}
		if (Text::StrSplit(strs, 3, strs2[1], ':') == 3)
		{
			if (strs[2][2] == '-' || strs[2][2] == '+')
			{
				Char c = strs[2][2];
				strs[2][2] = 0;
				SetTime(strs);

				if (Text::StrCharCnt(&strs[2][3]) == 5)
				{
					Int32 min = Text::StrToInt32(&strs[2][6]);
					if (strs[2][5] == ':')
					{
						strs[2][5] = 0;
					}
					else
					{
						strs[2][6] = 0;
					}
					min = min + Text::StrToInt32(&strs[2][3]) * 60;
					if (c == '-')
					{
						this->tzQhr = (Int8)(-min / 15);
					}
					else
					{
						this->tzQhr = (Int8)(min / 15);
					}
				}
			}
			else
			{
				if (Text::StrEndsWith(strs[2], "Z"))
				{
					strs[2][Text::StrCharCnt(strs[2]) - 1] = 0;
				}
				SetTime(strs);
			}
		}
		else
		{
			this->hour = 0;
			this->minute = 0;
			this->second = 0;
			this->ms = 0;
			if (!dateSucc)
			{
				succ = false;
			}
		}
	}
	else if (nStrs == 1)
	{
		if (Text::StrSplit(strs, 3, strs2[0], '-') == 3)
		{
			SetDate(strs);
			this->hour = 0;
			this->minute = 0;
			this->second = 0;
			this->ms = 0;
		}
		else if (Text::StrSplit(strs, 3, strs2[0], '/') == 3)
		{
			SetDate(strs);
			this->hour = 0;
			this->minute = 0;
			this->second = 0;
			this->ms = 0;
		}
		else if (Text::StrSplit(strs, 3, strs2[0], ':') == 3)
		{
			SetCurrTime();
			SetTime(strs);
		}
		else
		{
			succ = false;
		}
	}
	else if (nStrs == 4 || (nStrs == 5 && (strs2[4][0] == '-' || strs2[4][0] == '+' || Text::StrEquals(strs2[4], "GMT"))))
	{
		UOSInt len1 = Text::StrCharCnt(strs2[0]);
		UOSInt len2 = Text::StrCharCnt(strs2[1]);
		UOSInt len3 = Text::StrCharCnt(strs2[2]);
		UOSInt len4 = Text::StrCharCnt(strs2[3]);
		Char *timeStr = strs2[3];
		if (len1 == 3 && len2 <= 2 && len3 == 4)
		{
			Text::StrToUInt16(strs2[2], &this->year);
			this->month = Data::DateTime::ParseMonthStr(strs2[0]);
			this->day = Text::StrToUInt8(strs2[1]);
		}
		else if (len1 <= 2 && len2 == 3 && len3 == 4)
		{
			Text::StrToUInt16(strs2[2], &this->year);
			this->month = Data::DateTime::ParseMonthStr(strs2[1]);
			this->day = Text::StrToUInt8(strs2[0]);
		}
		else if (len1 == 3 && len2 <= 2 && len4 == 4)
		{
			Text::StrToUInt16(strs2[3], &this->year);
			this->month = Data::DateTime::ParseMonthStr(strs2[0]);
			this->day = Text::StrToUInt8(strs2[1]);
			timeStr = strs2[2];
		}
		else
		{
			succ = false;
		}
		if (Text::StrSplit(strs, 3, timeStr, ':') == 3)
		{
			SetTime(strs);
		}
		else
		{
			succ = false;
		}
		
		if (nStrs == 5)
		{
			if (Text::StrEquals(strs2[4], "GMT"))
			{
				this->tzQhr = 0;
			}
			else if (Text::StrCharCnt(strs2[4]) == 5)
			{
				Int32 min = Text::StrToInt32(&strs2[4][3]);
				if (strs2[4][2] == ':')
				{
					strs2[4][2] = 0;
				}
				else
				{
					strs2[4][3] = 0;
				}
				min = min + Text::StrToInt32(&strs2[4][1]) * 60;
				if (strs2[4][0] == '-')
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
		this->year = 1970;
		this->month = 1;
		this->day = 1;
		this->hour = 0;
		this->minute = 0;
		this->second = 0;
		this->ms = 0;
		this->tzQhr = 0;

		UOSInt j = 0;
		UOSInt i;
		UOSInt splitCnt;
		while (true)
		{
			if ((splitCnt = Text::StrSplit(strs, 3, strs2[j], ':')) == 3)
			{
				SetTime(strs);
			}
			else
			{
				if (splitCnt == 2)
				{
					strs[1][-1] = ':';
				}
				if (strs2[j][0] == '-')
				{
					if (Text::StrSplit(strs, 3, strs2[j], ':') == 2)
					{
						this->tzQhr = (Int8)(-((Text::StrToInt32(&strs[0][1]) << 2) + (Text::StrToInt32(strs[1]) / 15)));
					}
					else if (Text::StrCharCnt(strs2[j]) == 5)
					{
						this->tzQhr = (Int8)(Text::StrToInt32(&strs2[j][3]) / 15);
						strs2[j][3] = 0;
						this->tzQhr = (Int8)(-(this->tzQhr + (Text::StrToInt32(&strs2[j][1]) << 2)));
					}
				}
				else if (strs2[j][0] == '+')
				{
					if (Text::StrSplit(strs, 3, strs2[j], ':') == 2)
					{
						this->tzQhr = (Int8)((Text::StrToInt32(&strs[0][1]) << 2) + (Text::StrToInt32(strs[1]) / 15));
					}
					else if (Text::StrCharCnt(strs2[j]) == 5)
					{
						this->tzQhr = (Int8)(Text::StrToInt32(&strs2[j][3]) / 15);
						strs2[j][3] = 0;
						this->tzQhr = (Int8)(this->tzQhr + (Text::StrToInt32(&strs2[j][1]) << 2));
					}
				}
				else
				{
					i = Text::StrIndexOf(strs2[j], '/');
					if (i != INVALID_INDEX && i > 0)
					{
						if (Text::StrSplit(strs, 3, strs2[0], '/') == 3)
						{
							SetDate(strs);
						}
					}
					else
					{
						if (Text::StrSplit(strs, 3, strs2[0], '-') == 3)
						{
							SetDate(strs);
						}
						else
						{
							i = Text::StrToUInt32(strs2[j]);
							if (i <= 0)
							{
								i = ParseMonthStr(strs2[j]);
								if (i > 0)
								{
									this->month = (UInt8)i;
								}
							}
							else if (i > 100)
							{
								this->year = (UInt16)i;
							}
							else
							{
								this->day = (UInt8)i;
							}
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
				nStrs = Text::StrSplit(&strs2[1], 2, strs2[2], ' ');
			}
		}
	}
	return succ;
}

Bool Data::DateTime::SetValue(const UTF8Char *dateStr)
{
	return SetValue((const Char*)dateStr);
}

void Data::DateTime::SetValueSYSTEMTIME(void *sysTime)
{
	SYSTEMTIME *stime = (SYSTEMTIME*)sysTime;
	this->year = stime->wYear;
	this->month = (UInt8)stime->wMonth;
	this->day = (UInt8)stime->wDay;
	this->hour = (UInt8)stime->wHour;
	this->minute = (UInt8)stime->wMinute;
	this->second = (UInt8)stime->wSecond;
	this->ms = stime->wMilliseconds;
	this->tzQhr = 0;
}

void Data::DateTime::SetValueFILETIME(void *fileTime)
{
#if defined(_WIN32)
	FILETIME *ftime = (FILETIME*)fileTime;
	SYSTEMTIME stime;
	FileTimeToSystemTime(ftime, &stime);
	this->year = stime.wYear;
	this->month = (UInt8)stime.wMonth;
	this->day = (UInt8)stime.wDay;
	this->hour = (UInt8)stime.wHour;
	this->minute = (UInt8)stime.wMinute;
	this->second = (UInt8)stime.wSecond;
	this->ms = stime.wMilliseconds;
	this->tzQhr = 0;
#else
	this->SetTicks(ReadInt64((const UInt8*)fileTime) / 10000 - 11644473600000LL);
#endif
}

void Data::DateTime::SetValueVariTime(Double variTime)
{
	this->year = 1900;
	this->month = 1;
	this->day = 1;
	this->hour = 0;
	this->minute = 0;
	this->second = 0;
	this->ms = 0;
	this->tzQhr = 0;
	OSInt d = (OSInt)variTime;
	this->AddDay(d);
	variTime = (variTime - Math::OSInt2Double(d)) * 24;
	this->hour = (UInt8)variTime;
	variTime = (variTime - this->hour) * 60;
	this->minute = (UInt8)variTime;
	variTime = (variTime - this->minute) * 60;
	this->second = (UInt8)variTime;
	variTime = (variTime - this->second) * 1000;
	this->ms = (UInt16)variTime;
}

UInt16 Data::DateTime::GetYear()
{
	return this->year;
}

UInt8 Data::DateTime::GetMonth()
{
	return this->month;
}

UInt8 Data::DateTime::GetDay()
{
	return this->day;
}

UInt8 Data::DateTime::GetHour()
{
	return this->hour;
}

UInt8 Data::DateTime::GetMinute()
{
	return this->minute;
}

UInt8 Data::DateTime::GetSecond()
{
	return this->second;
}

UInt16 Data::DateTime::GetMS()
{
	return this->ms;
}

Data::DateTime *Data::DateTime::AddMonth(OSInt val)
{
	OSInt newMonth = this->month + val;
	while (newMonth < 1)
	{
		newMonth += 12;
		this->year--;
	}
	while (newMonth > 12)
	{
		newMonth -= 12;
		this->year++;
	}
	this->month = (UInt8)newMonth;
	UInt8 newDay = this->DayInMonth(this->year, (UInt8)newMonth);
	if (this->day > newDay)
	{
		this->day = newDay;
	}
	return this;
}

Data::DateTime *Data::DateTime::AddDay(OSInt val)
{
	OSInt newDay = this->day + val;
	OSInt dayim;
	if (newDay < 1)
	{
		while (newDay < 1)
		{
			if (--this->month <= 0)
			{
				this->year--;
				this->month = (UInt8)(this->month + 12);
			}
			newDay += this->DayInMonth(this->year, this->month);
		}
	}
	else
	{
		while (newDay > (dayim = this->DayInMonth(this->year, this->month)))
		{
			newDay = newDay - dayim;
			if (++this->month > 12)
			{
				this->year++;
				this->month = (UInt8)(this->month - 12);
			}
		}
	}
	this->day = (UInt8)newDay;
	return this;
}

Data::DateTime *Data::DateTime::AddHour(OSInt val)
{
	OSInt day = val / 24;
	OSInt outHour;
	outHour = val - day * 24 + this->hour;
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
	this->hour = (UInt8)outHour;
	if (day)
		AddDay(day);
	return this;
}

Data::DateTime *Data::DateTime::AddMinute(OSInt val)
{
	OSInt hours = val / 60;
	OSInt outMin;
	outMin = val - hours * 60 + this->minute;
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
	this->minute = (UInt8)outMin;
	if (hours)
		AddHour(hours);
	return this;
}

Data::DateTime *Data::DateTime::AddSecond(OSInt val)
{
	OSInt minutes = val / 60;
	OSInt outSec;
	outSec = val - minutes * 60 + this->second;
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
	this->second = (UInt8)outSec;
	if (minutes)
		AddMinute(minutes);
	return this;
}

Data::DateTime *Data::DateTime::AddMS(OSInt val)
{
	OSInt seconds = val / 1000;
	OSInt outMS;
	outMS = val - seconds * 1000 + this->ms;
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
	this->ms = (UInt16)outMS;
	if (seconds)
		AddSecond(seconds);
	return this;
}

void Data::DateTime::SetDate(UInt16 year, OSInt month, OSInt day)
{
	this->year = year;
	while (month < 1)
	{
		month += 12;
		this->year--;
	}
	while (month > 12)
	{
		month -= 12;
		this->year++;
	}
	this->month = (UInt8)month;
	OSInt dayim;
	if (day < 1)
	{
		while (day < 1)
		{
			if (--this->month <= 0)
			{
				this->year--;
				this->month = (UInt8)(this->month + 12);
			}
			day += this->DayInMonth(this->year, this->month);
		}
	}
	else
	{
		while (day > (dayim = this->DayInMonth(this->year, this->month)))
		{
			day = day - dayim;
			if (++this->month > 12)
			{
				this->year++;
				this->month = (UInt8)(this->month - 12);
			}
		}
	}
	this->day = (UInt8)day;
}

void Data::DateTime::SetYear(UInt16 year)
{
	this->year = year;
}

void Data::DateTime::SetMonth(OSInt month)
{
	this->SetDate(this->year, month, this->day);
}

void Data::DateTime::SetDay(OSInt day)
{
	this->SetDate(this->year, this->month, day);
}

void Data::DateTime::SetHour(OSInt hour)
{
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
	this->hour = (UInt8)hour;
	if (d != 0)
	{
		this->SetDate(this->year, this->month, this->day + d);
	}
}

void Data::DateTime::SetMinute(OSInt minute)
{
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
	this->minute = (UInt8)minute;
	if (h != 0)
	{
		this->SetHour(this->hour + h);
	}
}

void Data::DateTime::SetSecond(OSInt second)
{
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
	this->second = (UInt8)second;
	if (m != 0)
	{
		this->SetMinute(this->minute + m);
	}
}

void Data::DateTime::SetMS(OSInt ms)
{
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
	this->ms = (UInt16)ms;
	if (s != 0)
	{
		this->SetSecond(this->second + s);
	}
}

void Data::DateTime::ClearTime()
{
	this->hour = 0;
	this->minute = 0;
	this->second = 0;
	this->ms = 0;
}

Int64 Data::DateTime::GetMSPassedDate()
{
	return this->ms + this->second * 1000 + this->minute * 60000 + this->hour * 3600000;
}

Int64 Data::DateTime::DiffMS(DateTime *dt)
{
	return this->ToTicks() - dt->ToTicks();
}

Bool Data::DateTime::IsYearLeap()
{
	return (((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0);
}

Int64 Data::DateTime::ToTicks()
{
	Int32 totalDays;
	Int32 leapDays;
	Int32 yearDiff;
	Int32 yearDiff100;
	Int32 yearDiff400;

	Int32 currYear = this->year;
	Int32 currMonth = this->month;
	Int32 currDay = this->day;

	if (currYear <= 2000)
	{
		yearDiff = 2000 - currYear;
	}
	else
	{
		yearDiff = currYear - 2000 - 1;
	}
	yearDiff100 = yearDiff / 100;
	yearDiff400 = yearDiff100 >> 2;
	yearDiff >>= 2;
	leapDays = yearDiff - yearDiff100 + yearDiff400;

	if (currYear <= 2000)
	{
		totalDays = 10957 - (2000 - currYear) * 365 - leapDays;
	}
	else
	{
		totalDays = 10958 + (currYear - 2000) * 365 + leapDays;
	}

	switch (currMonth)
	{
	case 12:
		totalDays += 30;
	case 11:
		totalDays += 31;
	case 10:
		totalDays += 30;
	case 9:
		totalDays += 31;
	case 8:
		totalDays += 31;
	case 7:
		totalDays += 30;
	case 6:
		totalDays += 31;
	case 5:
		totalDays += 30;
	case 4:
		totalDays += 31;
	case 3:
		if (IsYearLeap())
			totalDays += 29;
		else
			totalDays += 28;
	case 2:
		totalDays += 31;
	case 1:
		break;
	default:
		break;
	}
	totalDays += currDay - 1;

	return totalDays * 86400000LL + (ms + second * 1000 + minute * 60000 + hour * 3600000 - this->tzQhr * 900000);
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
	ticks = ticks + this->tzQhr * 900000;
	Int32 totalDays = (Int32)(ticks / 86400000LL);
	UInt32 minutes = (UInt32)(ticks % 86400000LL);

	this->ms = (UInt16)(minutes % 1000);
	minutes = minutes / 1000;
	this->second = (UInt8)(minutes % 60);
	minutes = minutes / 60;
	this->minute = (UInt8)(minutes % 60);
	this->hour = (UInt8)(minutes / 60);

	if (totalDays < 0)
	{
		this->year = 1970;
		while (totalDays < 0)
		{
			this->year--;
			if (this->IsYearLeap())
			{
				totalDays += 366;
			}
			else
			{
				totalDays += 365;
			}
		}
	}
	else if (false && totalDays >= 10958)
	{
		this->year = 2000;
		totalDays -= 10958;
		while (true)
		{
			if (this->IsYearLeap())
			{
				if (totalDays < 366)
				{
					break;
				}
				else
				{
					this->year++;
					totalDays -= 366;
				}
			}
			else
			{
				if (totalDays < 365)
				{
					break;
				}
				else
				{
					this->year++;
					totalDays -= 365;
				}
			}
		}
	}
	else
	{
		this->year = 1970;
		while (true)
		{
			if (this->IsYearLeap())
			{
				if (totalDays < 366)
				{
					break;
				}
				else
				{
					this->year++;
					totalDays -= 366;
				}
			}
			else
			{
				if (totalDays < 365)
				{
					break;
				}
				else
				{
					this->year++;
					totalDays -= 365;
				}
			}
		}
	}

	Int32 febDays = 28;
	if (this->IsYearLeap())
		febDays = 29;

	if (totalDays < 31)
	{
		this->month = 1;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 31;
	if (totalDays < febDays)
	{
		this->month = 2;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= febDays;
	if (totalDays < 31)
	{
		this->month = 3;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 31;
	if (totalDays < 30)
	{
		this->month = 4;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 30;
	if (totalDays < 31)
	{
		this->month = 5;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 31;
	if (totalDays < 30)
	{
		this->month = 6;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 30;
	if (totalDays < 31)
	{
		this->month = 7;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 31;
	if (totalDays < 31)
	{
		this->month = 8;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 31;
	if (totalDays < 30)
	{
		this->month = 9;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 30;
	if (totalDays < 31)
	{
		this->month = 10;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 31;
	if (totalDays < 30)
	{
		this->month = 11;
		this->day = (UInt8)(totalDays + 1);
		return;
	}
	totalDays -= 30;
	this->month = 12;
	this->day = (UInt8)(totalDays + 1);
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
	this->year = (UInt16)(1980 + ((date >> 9) & 0x7f));
	this->month = (date >> 5) & 0xf;
	this->day = date & 0x1f;
	this->hour = (UInt8)(time >> 11);
	this->minute = (time >> 5) & 0x3f;
	this->second = (time & 0x1f) << 1;
	this->ms = 0;
}

UInt16 Data::DateTime::ToMSDOSDate()
{
	return (UInt16)((((this->year - 1980) & 0x7f) << 9) | (this->month << 5) | this->day);
}

UInt16 Data::DateTime::ToMSDOSTime()
{
	return (UInt16)((this->hour << 11) | (this->minute << 5) | (this->second >> 1));
}

void Data::DateTime::ToSYSTEMTIME(void *sysTime)
{
#if defined(WIN32) || defined(_WIN32_WCE)
	SYSTEMTIME *stime = (SYSTEMTIME*)sysTime;
	stime->wYear = this->year;
	stime->wMonth = this->month;
	stime->wDay = this->day;
	stime->wHour = this->hour;
	stime->wMinute = this->minute;
	stime->wSecond = this->second;
	stime->wMilliseconds = this->ms;
	stime->wDayOfWeek = 0;
#endif
}

void Data::DateTime::SetNTPTime(Int32 hiDword, Int32 loDword)
{
	this->ToUTCTime();
	this->year = 1900;
	this->month = 1;
	this->day = 1;
	this->hour = 0;
	this->minute = 0;
	this->second = 0;
	this->ms = 0;
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
	return ToString(buff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
}

Char *Data::DateTime::ToString(Char *buff, const Char *pattern)
{
	while (*pattern)
	{
		if (*pattern == 'y')
		{
			UInt16 thisVal = year;
			UInt8 digiCnt = 0;
			while (*pattern == 'y')
			{
				digiCnt++;
				pattern++;
			}
			buff += digiCnt;
			Char *src = buff;
			while (digiCnt-- > 0)
			{
				*--src = (Char)((thisVal % 10) + 0x30);
				thisVal = thisVal / 10;
			}
		}
		else if (*pattern == 'm')
		{
			if (pattern[1] != 'm')
			{
				if (minute < 10)
				{
					*buff++ = (Char)(minute + 0x30);
				}
				else
				{
					*buff++ = (Char)((minute / 10) + 0x30);
					*buff++ = (Char)((minute % 10) + 0x30);
				}
				pattern += 1;
			}
			else
			{
				*buff++ = (Char)((minute / 10) + 0x30);
				*buff++ = (Char)((minute % 10) + 0x30);

				while (*pattern == 'm')
					pattern++;
			}
		}
		else if (*pattern == 's')
		{
			if (pattern[1] != 's')
			{
				if (second < 10)
				{
					*buff++ = (Char)(second + 0x30);
				}
				else
				{
					*buff++ = (Char)((second / 10) + 0x30);
					*buff++ = (Char)((second % 10) + 0x30);
				}
				pattern += 1;
			}
			else
			{
				*buff++ = (Char)((second / 10) + 0x30);
				*buff++ = (Char)((second % 10) + 0x30);

				while (*pattern == 's')
					pattern++;
			}
		}
		else if (*pattern == 'd')
		{
			if (pattern[1] != 'd')
			{
				if (day < 10)
				{
					*buff++ = (Char)(day + 0x30);
				}
				else
				{
					*buff++ = 0x31;
					*buff++ = (Char)((day % 10) + 0x30);
				}
				pattern += 1;
			}
			else
			{
				*buff++ = (Char)((day / 10) + 0x30);
				*buff++ = (Char)((day % 10) + 0x30);

				while (*pattern == 'd')
					pattern++;
			}
		}
		else if (*pattern == 'f')
		{
			UInt8 digiCnt;
			UInt16 thisMS;
			if (pattern[1] != 'f')
			{
				digiCnt = 1;
				thisMS = ms / 100;
				pattern += 1;
			}
			else if (pattern[2] != 'f')
			{
				digiCnt = 2;
				thisMS = ms / 10;
				pattern += 2;
			}
			else
			{
				digiCnt = 3;
				thisMS = ms;
				pattern += 3;
			}

			buff += digiCnt;
			Char *src = buff;
			while (digiCnt-- > 0)
			{
				*--src = (Char)((thisMS % 10) + 0x30);
				thisMS = thisMS / 10;
			}
		}
		else if (*pattern == 'F')
		{
			UInt8 digiCnt;
			UInt16 thisMS;
			if (pattern[1] != 'F')
			{
				digiCnt = 1;
				thisMS = ms / 100;
				pattern += 1;
			}
			else if (pattern[2] != 'F')
			{
				digiCnt = 2;
				thisMS = ms / 10;
				pattern += 2;
			}
			else
			{
				digiCnt = 3;
				thisMS = ms ;
				pattern += 3;
			}

			while ((thisMS % 10) == 0)
			{
				thisMS = thisMS / 10;
				if (--digiCnt <= 0)
					break;
			}
			buff += digiCnt;
			Char *src = buff;
			while (digiCnt-- > 0)
			{
				*--src = (Char)((thisMS % 10) + 0x30);
				thisMS = thisMS / 10;
			}
		}
		else if (*pattern == 'h')
		{
			UInt8 thisH = hour % 12;
			if (pattern[1] != 'h')
			{
				if (thisH < 10)
				{
					*buff++ = (Char)(thisH + 0x30);
				}
				else
				{
					*buff++ = (Char)((thisH / 10) + 0x30);
					*buff++ = (Char)((thisH % 10) + 0x30);
				}
				pattern += 1;
			}
			else
			{
				*buff++ = (Char)((thisH / 10) + 0x30);
				*buff++ = (Char)((thisH % 10) + 0x30);

				while (*pattern == 'h')
					pattern++;
			}
		}
		else if (*pattern == 'H')
		{
			if (pattern[1] != 'H')
			{
				if (hour < 10)
				{
					*buff++ = (Char)(hour + 0x30);
				}
				else
				{
					*buff++ = (Char)((hour / 10) + 0x30);
					*buff++ = (Char)((hour % 10) + 0x30);
				}
				pattern += 1;
			}
			else
			{
				*buff++ = (Char)((hour / 10) + 0x30);
				*buff++ = (Char)((hour % 10) + 0x30);

				while (*pattern == 'H')
					pattern++;
			}
		}
		else if (*pattern == 'M')
		{
			if (pattern[1] != 'M')
			{
				if (month < 10)
				{
					*buff++ = (Char)(month + 0x30);
				}
				else
				{
					*buff++ = 0x31;
					*buff++ = (Char)(month + 38);
				}
				pattern += 1;
			}
			else if (pattern[2] != 'M')
			{
				if (month < 10)
				{
					*buff++ = 0x30;
					*buff++ = (Char)(month + 0x30);
				}
				else
				{
					*buff++ = 0x31;
					*buff++ = (Char)(month + 38);
				}
				pattern += 2;
			}
			else if (pattern[3] != 'M')
			{
				if (month == 1)
				{
					*buff++ = 'J';
					*buff++ = 'a';
					*buff++ = 'n';
				}
				else if (month == 2)
				{
					*buff++ = 'F';
					*buff++ = 'e';
					*buff++ = 'b';
				}
				else if (month == 3)
				{
					*buff++ = 'M';
					*buff++ = 'a';
					*buff++ = 'r';
				}
				else if (month == 4)
				{
					*buff++ = 'A';
					*buff++ = 'p';
					*buff++ = 'r';
				}
				else if (month == 5)
				{
					*buff++ = 'M';
					*buff++ = 'a';
					*buff++ = 'y';
				}
				else if (month == 6)
				{
					*buff++ = 'J';
					*buff++ = 'u';
					*buff++ = 'n';
				}
				else if (month == 7)
				{
					*buff++ = 'J';
					*buff++ = 'u';
					*buff++ = 'l';
				}
				else if (month == 8)
				{
					*buff++ = 'A';
					*buff++ = 'u';
					*buff++ = 'g';
				}
				else if (month == 9)
				{
					*buff++ = 'S';
					*buff++ = 'e';
					*buff++ = 'p';
				}
				else if (month == 10)
				{
					*buff++ = 'O';
					*buff++ = 'c';
					*buff++ = 't';
				}
				else if (month == 11)
				{
					*buff++ = 'N';
					*buff++ = 'o';
					*buff++ = 'v';
				}
				else if (month == 12)
				{
					*buff++ = 'D';
					*buff++ = 'e';
					*buff++ = 'c';
				}
				pattern += 3;
			}
			else
			{
				if (month == 1)
				{
					*buff++ = 'J';
					*buff++ = 'a';
					*buff++ = 'n';
					*buff++ = 'u';
					*buff++ = 'a';
					*buff++ = 'r';
					*buff++ = 'y';
				}
				else if (month == 2)
				{
					*buff++ = 'F';
					*buff++ = 'e';
					*buff++ = 'b';
					*buff++ = 'r';
					*buff++ = 'u';
					*buff++ = 'a';
					*buff++ = 'r';
					*buff++ = 'y';
				}
				else if (month == 3)
				{
					*buff++ = 'M';
					*buff++ = 'a';
					*buff++ = 'r';
					*buff++ = 'c';
					*buff++ = 'h';
				}
				else if (month == 4)
				{
					*buff++ = 'A';
					*buff++ = 'p';
					*buff++ = 'r';
					*buff++ = 'i';
					*buff++ = 'l';
				}
				else if (month == 5)
				{
					*buff++ = 'M';
					*buff++ = 'a';
					*buff++ = 'y';
				}
				else if (month == 6)
				{
					*buff++ = 'J';
					*buff++ = 'u';
					*buff++ = 'n';
					*buff++ = 'e';
				}
				else if (month == 7)
				{
					*buff++ = 'J';
					*buff++ = 'u';
					*buff++ = 'l';
					*buff++ = 'y';
				}
				else if (month == 8)
				{
					*buff++ = 'A';
					*buff++ = 'u';
					*buff++ = 'g';
					*buff++ = 'u';
					*buff++ = 's';
					*buff++ = 't';
				}
				else if (month == 9)
				{
					*buff++ = 'S';
					*buff++ = 'e';
					*buff++ = 'p';
					*buff++ = 't';
					*buff++ = 'e';
					*buff++ = 'm';
					*buff++ = 'b';
					*buff++ = 'e';
					*buff++ = 'r';
				}
				else if (month == 10)
				{
					*buff++ = 'O';
					*buff++ = 'c';
					*buff++ = 't';
					*buff++ = 'o';
					*buff++ = 'b';
					*buff++ = 'e';
					*buff++ = 'r';
				}
				else if (month == 11)
				{
					*buff++ = 'N';
					*buff++ = 'o';
					*buff++ = 'v';
					*buff++ = 'e';
					*buff++ = 'm';
					*buff++ = 'b';
					*buff++ = 'e';
					*buff++ = 'r';
				}
				else if (month == 12)
				{
					*buff++ = 'D';
					*buff++ = 'e';
					*buff++ = 'c';
					*buff++ = 'e';
					*buff++ = 'm';
					*buff++ = 'b';
					*buff++ = 'e';
					*buff++ = 'r';
				}

				while (*pattern == 'M')
					pattern++;
			}
		}
		else if (*pattern == 't')
		{
			if (pattern[1] != 't')
			{
				if (hour >= 12)
				{
					*buff++ = 'P';
				}
				else
				{
					*buff++ = 'A';
				}
				pattern += 1;
			}
			else
			{
				if (hour >= 12)
				{
					*buff++ = 'P';
				}
				else
				{
					*buff++ = 'A';
				}
				*buff++ = 'M';
				while (*pattern == 't')
					pattern++;
			}
		}
		else if (*pattern == 'z')
		{
			Int32 hr = this->tzQhr >> 2;
			Int32 min = (this->tzQhr & 3) * 15;
			if (pattern[1] != 'z')
			{
				if (hr >= 0)
				{
					*buff++ = '+';
				}
				else
				{
					*buff++ = '-';
					hr = -hr;
				}
				if (hr >= 10)
				{
					*buff++ = (Char)((hr / 10) + 0x30);
					*buff++ = (Char)((hr % 10) + 0x30);
				}
				else
				{
					*buff++ = (Char)(hr + 0x30);
				}
				pattern++;
			}
			else if (pattern[2] != 'z')
			{
				if (hr >= 0)
				{
					*buff++ = '+';
				}
				else
				{
					*buff++ = '-';
					hr = -hr;
				}
				*buff++ = (Char)((hr / 10) + 0x30);
				*buff++ = (Char)((hr % 10) + 0x30);
				pattern += 2;
			}
			else if (pattern[3] != 'z')
			{
				if (hr >= 0)
				{
					*buff++ = '+';
				}
				else
				{
					*buff++ = '-';
					hr = -hr;
				}
				*buff++ = (Char)((hr / 10) + 0x30);
				*buff++ = (Char)((hr % 10) + 0x30);
				*buff++ = (Char)((min / 10) + 0x30);
				*buff++ = (Char)((min % 10) + 0x30);
				pattern += 3;
			}
			else
			{
				if (hr >= 0)
				{
					*buff++ = '+';
				}
				else
				{
					*buff++ = '-';
					hr = -hr;
				}
				*buff++ = (Char)((hr / 10) + 0x30);
				*buff++ = (Char)((hr % 10) + 0x30);
				*buff++ = ':';
				*buff++ = (Char)((min / 10) + 0x30);
				*buff++ = (Char)((min % 10) + 0x30);
				
				while (*pattern == 'z')
					pattern++;
			}
		}
		else if (*pattern == '\\')
		{
			if (pattern[1] == 0)
				*buff++ = *pattern++;
			else
			{
				pattern++;
				*buff++ = *pattern++;
			}
		}
		else
		{
			*buff++ = *pattern++;
		}
	}
	*buff = 0;
	return buff;
}

UTF8Char *Data::DateTime::ToString(UTF8Char *buff)
{
	return (UTF8Char*)ToString((Char*)buff);
}

UTF8Char *Data::DateTime::ToString(UTF8Char *buff, const Char *pattern)
{
	return (UTF8Char*)ToString((Char*)buff, pattern);
}

Data::DateTime Data::DateTime::operator=(Data::DateTime dt)
{
	this->year = dt.year;
	this->month = dt.month;
	this->day = dt.day;
	this->hour = dt.hour;
	this->minute = dt.minute;
	this->second = dt.second;
	this->ms = dt.ms;
	this->tzQhr = dt.tzQhr;
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
	t.tm_year = this->year;
	t.tm_isdst = false;
	t.tm_mon = this->month - 1;
	t.tm_mday = this->day;
	t.tm_hour = this->hour;
	t.tm_min = this->minute;
	t.tm_sec = this->second;
	return &buff[strftime((char*)buff, 100, "%c", &t)];
#else
	return 0;
#endif
}

OSInt Data::DateTime::CompareTo(Data::IComparable *obj)
{
	Data::DateTime *dt;
	dt = (Data::DateTime*)obj;
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
	if (this->year < dt->year)
		return -1;
	else if (this->year > dt->year)
		return 1;
	else if (this->month < dt->month)
		return -1;
	else if (this->month > dt->month)
		return 1;
	else if (this->day < dt->day)
		return -1;
	else if (this->day > dt->day)
		return 1;
	else
		return 0;
}

Bool Data::DateTime::IsSameDay(Data::DateTime *dt)
{
	return this->year == dt->year && this->month == dt->month && this->day == dt->day;
}

UInt8 Data::DateTime::ParseMonthStr(const Char *month)
{
	if (Text::StrStartsWithICase(month, "JAN"))
	{
		return 1;
	}
	else if (Text::StrStartsWithICase(month, "FEB"))
	{
		return 2;
	}
	else if (Text::StrStartsWithICase(month, "MAR"))
	{
		return 3;
	}
	else if (Text::StrStartsWithICase(month, "APR"))
	{
		return 4;
	}
	else if (Text::StrStartsWithICase(month, "MAY"))
	{
		return 5;
	}
	else if (Text::StrStartsWithICase(month, "JUN"))
	{
		return 6;
	}
	else if (Text::StrStartsWithICase(month, "JUL"))
	{
		return 7;
	}
	else if (Text::StrStartsWithICase(month, "AUG"))
	{
		return 8;
	}
	else if (Text::StrStartsWithICase(month, "SEP"))
	{
		return 9;
	}
	else if (Text::StrStartsWithICase(month, "OCT"))
	{
		return 10;
	}
	else if (Text::StrStartsWithICase(month, "NOV"))
	{
		return 11;
	}
	else if (Text::StrStartsWithICase(month, "DEC"))
	{
		return 12;
	}
	return 0;
}

UInt8 Data::DateTime::ParseMonthStr(const UTF8Char *month)
{
	return ParseMonthStr((const Char*)month);
}

void Data::DateTime::ToUTCTime()
{
	if (this->tzQhr != 0)
	{
		Int32 tzQhr = this->tzQhr;
		this->tzQhr = 0;
		this->AddMinute(tzQhr * -15);
	}
}

void Data::DateTime::ToLocalTime()
{
	Int8 newTZ = GetLocalTzQhr();
	if (this->tzQhr != newTZ)
	{
		Int32 tzv = this->tzQhr - newTZ;
		this->tzQhr = newTZ;
		this->AddMinute(tzv * -15);
	}
}

void Data::DateTime::ConvertTimeZoneQHR(Int8 tzQhr)
{
	if (this->tzQhr != tzQhr)
	{
		Int32 tzv = this->tzQhr - tzQhr;
		this->tzQhr = tzQhr;
		this->AddMinute(tzv * -15);
	}
}

void Data::DateTime::SetTimeZoneQHR(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

Int8 Data::DateTime::GetTimeZoneQHR()
{
	return this->tzQhr;
}

Data::DateTime::Weekday Data::DateTime::GetWeekday()
{
	return (Data::DateTime::Weekday)(((this->ToUnixTimestamp() + this->tzQhr * 900) / 86400 + 4) % 7);
}

Double Data::DateTime::MS2Days(Int64 ms)
{
	return (Double)ms / 86400000.0;
}

Double Data::DateTime::MS2Hours(Int64 ms)
{
	return (Double)ms / 3600000.0;
}

Double Data::DateTime::MS2Minutes(Int64 ms)
{
	return (Double)ms / 60000.0;
}

Double Data::DateTime::MS2Seconds(Int64 ms)
{
	return (Double)ms * 0.001;
}

UInt8 Data::DateTime::DayInMonth(UInt16 year, UInt8 month)
{
	while (month < 1)
	{
		month = (UInt8)(month + 12);
		year--;
	}
	while (month > 12)
	{
		month = (UInt8)(month - 12);
		year++;
	}
	switch (month)
	{
	case 12:
		return 31;
	case 11:
		return 30;
	case 10:
		return 31;
	case 9:
		return 30;
	case 8:
		return 31;
	case 7:
		return 31;
	case 6:
		return 30;
	case 5:
		return 31;
	case 4:
		return 30;
	case 3:
		return 31;
	case 2:
		if (((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0)
			return 29;
		else
			return 28;
	case 1:
		return 31;
	default:
		return 0;
	}
}

Int8 Data::DateTime::GetLocalTzQhr()
{
#if defined(WIN32) || defined(_WIN32_WCE)
	TIME_ZONE_INFORMATION tz;
	tz.Bias = 0;
	GetTimeZoneInformation(&tz);
	Int32 newTZ = tz.Bias / -15;
#elif defined(__sun__)
	time_t now = time(0);
	tm *t = localtime(&now);
	Int32 newTZ = mktime(t) - now;
	if (t->tm_isdst > 0)
	{
    	newTZ = newTZ - 60 * 60;
	}
	newTZ = newTZ / 900;
#elif !defined(CPU_AVR)
	time_t now = time(0);
	tm *t = localtime(&now);
	Int32 newTZ = (Int32)(t->tm_gmtoff / 900);
#else
	Int32 newTZ = 0;
#endif
	return (Int8)newTZ;
}
