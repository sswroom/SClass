#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/DateTimeUtil.h"
#include "Data/TimeInstant.h"
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

const Char *Data::DateTimeUtil::monString[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const Char *Data::DateTimeUtil::monthString[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
Int8 Data::DateTimeUtil::localTzQhr = 0;
Bool Data::DateTimeUtil::localTzValid = false;

Bool Data::DateTimeUtil::DateValueSetDate(NN<Data::DateTimeUtil::DateValue> t, Text::PString *dateStrs)
{
	UInt32 vals[3];
	vals[0] = 0;
	vals[1] = 0;
	vals[2] = 0;
	if (!Text::StrToUInt32(dateStrs[0].v, vals[0]) ||
		!Text::StrToUInt32(dateStrs[1].v, vals[1]) ||
		!Text::StrToUInt32(dateStrs[2].v, vals[2]))
	return false;
	if (vals[0] > 100 || dateStrs[0].leng >= 4)
	{
		t->year = (UInt16)vals[0];
		t->month = (UInt8)vals[1];
		t->day = (UInt8)vals[2];
	}
	else if (vals[2] > 100 || dateStrs[2].leng >= 4)
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
			t->year = (((t->year / 100) * 100) + (Int32)vals[2]);
			t->month = (UInt8)vals[0];
			t->day = (UInt8)vals[1];
		}
		else
		{
			t->year = (((t->year / 100) * 100) + (Int32)vals[0]);
			t->month = (UInt8)vals[1];
			t->day = (UInt8)vals[2];
		}
	}
	return true;
}

Bool Data::DateTimeUtil::TimeValueSetTime(NN<Data::DateTimeUtil::TimeValue> t, Text::PString *timeStrs, OutParam<UInt32> nanosec)
{
	Text::PString strs[2];
	UIntOS valTmp;

	t->hour = (UInt8)Text::StrToUInt32(timeStrs[0].v);
	t->minute = (UInt8)Text::StrToUInt32(timeStrs[1].v);
	valTmp = Text::StrSplitP(strs, 2, timeStrs[2], '.');
	if (valTmp == 1)
	{
		t->second = (UInt8)Text::StrToUInt32(strs[0].v);
		nanosec.Set(0);
	}
	else
	{
		t->second = (UInt8)Text::StrToUInt32(strs[0].v);
		valTmp = strs[1].leng;
		switch (strs[1].leng)
		{
		case 0:
			nanosec.Set(0);
			break;
		case 1:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 100000000);
			break;
		case 2:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 10000000);
			break;
		case 3:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 1000000);
			break;
		case 4:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 100000);
			break;
		case 5:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 10000);
			break;
		case 6:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 1000);
			break;
		case 7:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 100);
			break;
		case 8:
			nanosec.Set(Text::StrToUInt32(strs[1].v) * 10);
			break;
		default:
			strs[1].v[9] = 0;
			nanosec.Set(Text::StrToUInt32(strs[1].v));
			break;
		}
	}
	return true;
}

Int64 Data::DateTimeUtil::Date2TotalDays(Int32 year, Int32 month, Int32 day)
{
	Int32 totalDays;
	Int32 leapDays;
	Int32 yearDiff;
	Int32 yearDiff100;
	Int32 yearDiff400;

	Int32 currYear = year;
	Int32 currMonth = month;
	Int32 currDay = day;
	if (currMonth < 1)
	{
		yearDiff = (-currMonth + 12) / 12;
		currMonth += yearDiff * 12;
		currYear -= yearDiff;
	}
	else if (currMonth > 12)
	{
		yearDiff = (currMonth - 1) / 12;
		currMonth -= yearDiff * 12;
		currYear += yearDiff;
	}
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
		if (IsYearLeap(year))
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
	return totalDays;
}

Int64 Data::DateTimeUtil::DateValue2TotalDays(NN<const DateValue> d)
{
	return Date2TotalDays(d->year, d->month, d->day);
}

Int64 Data::DateTimeUtil::TimeValue2Secs(NN<const TimeValue> t, Int8 tzQhr)
{
	return DateValue2TotalDays(t) * 86400LL + (t->second + t->minute * 60 + t->hour * 3600 - tzQhr * 900);
}

Int64 Data::DateTimeUtil::TimeValue2Ticks(NN<const TimeValue> t, UInt32 ns, Int8 tzQhr)
{
	return TimeValue2Secs(t, tzQhr) * 1000LL + (ns / 1000000);
}

void Data::DateTimeUtil::Ticks2TimeValue(Int64 ticks, NN<TimeValue> t, Int8 tzQhr)
{
	Secs2TimeValue(ticks / 1000, t, tzQhr);
}

void Data::DateTimeUtil::Secs2TimeValue(Int64 secs, NN<TimeValue> t, Int8 tzQhr)
{
	secs = secs + tzQhr * 900;
	Int32 totalDays = (Int32)(secs / 86400LL);
	UInt32 minutes;
	if (secs < 0)
	{
		secs -= totalDays * 86400LL;
		while (secs < 0)
		{
			totalDays -= 1;
			secs += 86400LL;
		}
		minutes = (UInt32)(secs % 86400LL);
	}
	else
	{
		minutes = (UInt32)(secs % 86400LL);
	}

	t->second = (UInt8)(minutes % 60);
	minutes = minutes / 60;
	t->minute = (UInt8)(minutes % 60);
	t->hour = (UInt8)(minutes / 60);

	if (totalDays < 0)
	{
		t->year = 1970;
		while (totalDays < 0)
		{
			t->year--;
			if (IsYearLeap(t->year))
			{
				totalDays += 366;
			}
			else
			{
				totalDays += 365;
			}
		}
	}
	else
	{
		if (totalDays < 10957)
		{
			t->year = 1970;
			while (true)
			{
				if (IsYearLeap(t->year))
				{
					if (totalDays < 366)
					{
						break;
					}
					else
					{
						t->year++;
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
						t->year++;
						totalDays -= 365;
					}
				}
			}
		}
		else
		{
			totalDays -= 10957;
			t->year = (2000 + ((totalDays / 1461) << 2));
			totalDays = totalDays % 1461;
			if (totalDays >= 366)
			{
				totalDays--;
				t->year = (t->year + totalDays / 365);
				totalDays = totalDays % 365;
			}
		}
	}

	if (IsYearLeap(t->year))
	{
		if (totalDays < 121)
		{
			if (totalDays < 60)
			{
				if (totalDays < 31)
				{
					t->month = 1;
					t->day = (UInt8)(totalDays + 1);
				}
				else
				{
					t->month = 2;
					t->day = (UInt8)(totalDays - 31 + 1);
				}
			}
			else
			{
				if (totalDays < 91)
				{
					t->month = 3;
					t->day = (UInt8)(totalDays - 60 + 1);
				}
				else
				{
					t->month = 4;
					t->day = (UInt8)(totalDays - 91 + 1);
				}
			}
		}
		else
		{
			if (totalDays < 244)
			{
				if (totalDays < 182)
				{
					if (totalDays < 152)
					{
						t->month = 5;
						t->day = (UInt8)(totalDays - 121 + 1);
					}
					else
					{
						t->month = 6;
						t->day = (UInt8)(totalDays - 152 + 1);
					}
				}
				else
				{
					if (totalDays < 213)
					{
						t->month = 7;
						t->day = (UInt8)(totalDays - 182 + 1);
					}
					else
					{
						t->month = 8;
						t->day = (UInt8)(totalDays - 213 + 1);
					}
				}
			}
			else
			{
				if (totalDays < 305)
				{
					if (totalDays < 274)
					{
						t->month = 9;
						t->day = (UInt8)(totalDays - 244 + 1);
					}
					else
					{
						t->month = 10;
						t->day = (UInt8)(totalDays - 274 + 1);
					}
				}
				else
				{
					if (totalDays < 335)
					{
						t->month = 11;
						t->day = (UInt8)(totalDays - 305 + 1);
					}
					else
					{
						t->month = 12;
						t->day = (UInt8)(totalDays - 335 + 1);
					}
				}
			}
		}
	}
	else
	{
		if (totalDays < 120)
		{
			if (totalDays < 59)
			{
				if (totalDays < 31)
				{
					t->month = 1;
					t->day = (UInt8)(totalDays + 1);
				}
				else
				{
					t->month = 2;
					t->day = (UInt8)(totalDays - 31 + 1);
				}
			}
			else
			{
				if (totalDays < 90)
				{
					t->month = 3;
					t->day = (UInt8)(totalDays - 59 + 1);
				}
				else
				{
					t->month = 4;
					t->day = (UInt8)(totalDays - 90 + 1);
				}
			}
		}
		else
		{
			if (totalDays < 243)
			{
				if (totalDays < 181)
				{
					if (totalDays < 151)
					{
						t->month = 5;
						t->day = (UInt8)(totalDays - 120 + 1);
					}
					else
					{
						t->month = 6;
						t->day = (UInt8)(totalDays - 151 + 1);
					}
				}
				else
				{
					if (totalDays < 212)
					{
						t->month = 7;
						t->day = (UInt8)(totalDays - 181 + 1);
					}
					else
					{
						t->month = 8;
						t->day = (UInt8)(totalDays - 212 + 1);
					}
				}
			}
			else
			{
				if (totalDays < 304)
				{
					if (totalDays < 273)
					{
						t->month = 9;
						t->day = (UInt8)(totalDays - 243 + 1);
					}
					else
					{
						t->month = 10;
						t->day = (UInt8)(totalDays - 273 + 1);
					}
				}
				else
				{
					if (totalDays < 334)
					{
						t->month = 11;
						t->day = (UInt8)(totalDays - 304 + 1);
					}
					else
					{
						t->month = 12;
						t->day = (UInt8)(totalDays - 334 + 1);
					}
				}
			}
		}
	}
}

void Data::DateTimeUtil::TotalDays2DateValue(Int64 totalDays, NN<DateValue> d)
{
	if (totalDays < 0)
	{
		d->year = 1970;
		while (totalDays < 0)
		{
			d->year--;
			if (IsYearLeap(d->year))
			{
				totalDays += 366;
			}
			else
			{
				totalDays += 365;
			}
		}
	}
	else
	{
		if (totalDays < 10957)
		{
			d->year = 1970;
			while (true)
			{
				if (IsYearLeap(d->year))
				{
					if (totalDays < 366)
					{
						break;
					}
					else
					{
						d->year++;
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
						d->year++;
						totalDays -= 365;
					}
				}
			}
		}
		else
		{
			totalDays -= 10957;
			d->year = (UInt16)(2000 + ((totalDays / 1461) << 2));
			totalDays = totalDays % 1461;
			if (totalDays >= 366)
			{
				totalDays--;
				d->year = (UInt16)(d->year + totalDays / 365);
				totalDays = totalDays % 365;
			}
		}
	}

	if (IsYearLeap(d->year))
	{
		if (totalDays < 121)
		{
			if (totalDays < 60)
			{
				if (totalDays < 31)
				{
					d->month = 1;
					d->day = (UInt8)(totalDays + 1);
				}
				else
				{
					d->month = 2;
					d->day = (UInt8)(totalDays - 31 + 1);
				}
			}
			else
			{
				if (totalDays < 91)
				{
					d->month = 3;
					d->day = (UInt8)(totalDays - 60 + 1);
				}
				else
				{
					d->month = 4;
					d->day = (UInt8)(totalDays - 91 + 1);
				}
			}
		}
		else
		{
			if (totalDays < 244)
			{
				if (totalDays < 182)
				{
					if (totalDays < 152)
					{
						d->month = 5;
						d->day = (UInt8)(totalDays - 121 + 1);
					}
					else
					{
						d->month = 6;
						d->day = (UInt8)(totalDays - 152 + 1);
					}
				}
				else
				{
					if (totalDays < 213)
					{
						d->month = 7;
						d->day = (UInt8)(totalDays - 182 + 1);
					}
					else
					{
						d->month = 8;
						d->day = (UInt8)(totalDays - 213 + 1);
					}
				}
			}
			else
			{
				if (totalDays < 305)
				{
					if (totalDays < 274)
					{
						d->month = 9;
						d->day = (UInt8)(totalDays - 244 + 1);
					}
					else
					{
						d->month = 10;
						d->day = (UInt8)(totalDays - 274 + 1);
					}
				}
				else
				{
					if (totalDays < 335)
					{
						d->month = 11;
						d->day = (UInt8)(totalDays - 305 + 1);
					}
					else
					{
						d->month = 12;
						d->day = (UInt8)(totalDays - 335 + 1);
					}
				}
			}
		}
	}
	else
	{
		if (totalDays < 120)
		{
			if (totalDays < 59)
			{
				if (totalDays < 31)
				{
					d->month = 1;
					d->day = (UInt8)(totalDays + 1);
				}
				else
				{
					d->month = 2;
					d->day = (UInt8)(totalDays - 31 + 1);
				}
			}
			else
			{
				if (totalDays < 90)
				{
					d->month = 3;
					d->day = (UInt8)(totalDays - 59 + 1);
				}
				else
				{
					d->month = 4;
					d->day = (UInt8)(totalDays - 90 + 1);
				}
			}
		}
		else
		{
			if (totalDays < 243)
			{
				if (totalDays < 181)
				{
					if (totalDays < 151)
					{
						d->month = 5;
						d->day = (UInt8)(totalDays - 120 + 1);
					}
					else
					{
						d->month = 6;
						d->day = (UInt8)(totalDays - 151 + 1);
					}
				}
				else
				{
					if (totalDays < 212)
					{
						d->month = 7;
						d->day = (UInt8)(totalDays - 181 + 1);
					}
					else
					{
						d->month = 8;
						d->day = (UInt8)(totalDays - 212 + 1);
					}
				}
			}
			else
			{
				if (totalDays < 304)
				{
					if (totalDays < 273)
					{
						d->month = 9;
						d->day = (UInt8)(totalDays - 243 + 1);
					}
					else
					{
						d->month = 10;
						d->day = (UInt8)(totalDays - 273 + 1);
					}
				}
				else
				{
					if (totalDays < 334)
					{
						d->month = 11;
						d->day = (UInt8)(totalDays - 304 + 1);
					}
					else
					{
						d->month = 12;
						d->day = (UInt8)(totalDays - 334 + 1);
					}
				}
			}
		}
	}
}

void Data::DateTimeUtil::Instant2TimeValue(Int64 secs, UInt32 nanosec, NN<TimeValue> t, Int8 tzQhr)
{
	secs = secs + tzQhr * 900;
	Int32 totalDays = (Int32)(secs / 86400LL);
	UInt32 minutes;
	if (secs < 0)
	{
		secs -= totalDays * 86400LL;
		while (secs < 0)
		{
			totalDays -= 1;
			secs += 86400LL;
		}
		minutes = (UInt32)(secs % 86400LL);
	}
	else
	{
		minutes = (UInt32)(secs % 86400LL);
	}

	t->second = (UInt8)(minutes % 60);
	minutes = minutes / 60;
	t->minute = (UInt8)(minutes % 60);
	t->hour = (UInt8)(minutes / 60);
	TotalDays2DateValue(totalDays, t);
}

Data::DateTimeUtil::Weekday Data::DateTimeUtil::Ticks2Weekday(Int64 ticks, Int8 tzQhr)
{
	Int64 days = ((ticks + tzQhr * 900000) / 86400000 + 4);
	if (days >= 0)
	{
		return (Data::DateTimeUtil::Weekday)(days % 7);
	}
	else
	{
		return (Data::DateTimeUtil::Weekday)((days % 7) + 7);
	}
}

Data::DateTimeUtil::Weekday Data::DateTimeUtil::Instant2Weekday(Data::TimeInstant inst, Int8 tzQhr)
{
	return (Data::DateTimeUtil::Weekday)(((inst.sec + tzQhr * 900) / 86400 + 4) % 7);
}

UnsafeArray<UTF8Char> Data::DateTimeUtil::ToString(UnsafeArray<UTF8Char> sbuff, NN<const TimeValue> tval, Int8 tzQhr, UInt32 nanosec, UnsafeArray<const UTF8Char> pattern)
{
	while (*pattern)
	{
		switch (*pattern)
		{
		case 'y':
		{
			Int32 thisVal = tval->year;
			UInt32 neg;
			UInt8 digiCnt = 1;
			if (thisVal <= 0)
			{
				neg = 1;
				thisVal = -thisVal + 1;
			}
			else
			{
				neg = 0;
			}
			pattern++;
			while (*pattern == 'y')
			{
				digiCnt++;
				pattern++;
			}
			sbuff += digiCnt + neg;
			UnsafeArray<UTF8Char> src = sbuff;
			while (digiCnt >= 2)
			{
				src -= 2;
				WriteNUInt16(&src[0], ReadNUInt16(&MyString_StrDigit100U8[(thisVal % 100) * 2]));
				thisVal = thisVal / 100;
				digiCnt = (UInt8)(digiCnt - 2);
			}
			if (digiCnt > 0)
			{
				*--src = (UTF8Char)((thisVal % 10) + 0x30);
			}
			if (neg)
			{
				*--src = '-';
			}
			break;
		}
		case 'm':
		{
			pattern++;
			if (*pattern != 'm')
			{
				if (tval->minute < 10)
				{
					*sbuff++ = (UTF8Char)(tval->minute + 0x30);
				}
				else
				{
					WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->minute * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->minute * 2]));
				sbuff += 2;

				pattern++;
//				while (*pattern == 'm')
//					pattern++;
			}
			break;
		}
		case 's':
		{
			pattern++;
			if (*pattern != 's')
			{
				if (tval->second < 10)
				{
					*sbuff++ = (UTF8Char)(tval->second + 0x30);
				}
				else
				{
					WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->second * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->second * 2]));
				sbuff += 2;

				pattern++;
//				while (*pattern == 's')
//					pattern++;
			}
			break;
		}
		case 'd':
		{
			pattern++;
			if (*pattern != 'd')
			{
				if (tval->day < 10)
				{
					*sbuff++ = (UTF8Char)(tval->day + 0x30);
				}
				else
				{
					WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->day * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->day * 2]));
				sbuff += 2;

				pattern++;
//				while (*pattern == 'd')
//					pattern++;
			}
			break;
		}
		case 'f':
		{
			UInt32 sv;
			if (pattern[1] != 'f')
			{
				*sbuff = (UTF8Char)((nanosec / 100000000) + 0x30);
				pattern += 1;
				sbuff++;
			}
			else if (pattern[2] != 'f')
			{
				sv = nanosec / 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff += 2;
				pattern += 2;
			}
			else if (pattern[3] != 'f')
			{
				sv = nanosec / 10000000;
				nanosec = nanosec % 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff[2] = (UTF8Char)((nanosec / 1000000) + 0x30);
				sbuff += 3;
				pattern += 3;
			}
			else if (pattern[4] != 'f')
			{
				sv = nanosec / 10000000;
				nanosec = nanosec % 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 100000;
				WriteNUInt16(&sbuff[2], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff += 4;
				pattern += 4;
			}
			else if (pattern[5] != 'f')
			{
				sv = nanosec / 10000000;
				nanosec = nanosec % 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 100000;
				nanosec = nanosec % 100000;
				WriteNUInt16(&sbuff[2], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff[4] = (UTF8Char)((nanosec / 10000) + 0x30);
				sbuff += 5;
				pattern += 5;
			}
			else if (pattern[6] != 'f')
			{
				sv = nanosec / 10000000;
				nanosec = nanosec % 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 100000;
				nanosec = nanosec % 100000;
				WriteNUInt16(&sbuff[2], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 1000;
				WriteNUInt16(&sbuff[4], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff += 6;
				pattern += 6;
			}
			else if (pattern[7] != 'f')
			{
				sv = nanosec / 10000000;
				nanosec = nanosec % 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 100000;
				nanosec = nanosec % 100000;
				WriteNUInt16(&sbuff[2], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 1000;
				nanosec = nanosec % 1000;
				WriteNUInt16(&sbuff[4], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff[6] = (UTF8Char)((nanosec / 100) + 0x30);
				sbuff += 7;
				pattern += 7;
			}
			else if (pattern[8] != 'f')
			{
				sv = nanosec / 10000000;
				nanosec = nanosec % 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 100000;
				nanosec = nanosec % 100000;
				WriteNUInt16(&sbuff[2], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 1000;
				nanosec = nanosec % 1000;
				WriteNUInt16(&sbuff[4], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 10;
				WriteNUInt16(&sbuff[6], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff += 8;
				pattern += 8;
			}
			else
			{
				sv = nanosec / 10000000;
				nanosec = nanosec % 10000000;
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 100000;
				nanosec = nanosec % 100000;
				WriteNUInt16(&sbuff[2], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 1000;
				nanosec = nanosec % 1000;
				WriteNUInt16(&sbuff[4], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sv = nanosec / 10;
				nanosec = nanosec % 10;
				WriteNUInt16(&sbuff[6], ReadNUInt16(&MyString_StrDigit100U8[sv * 2]));
				sbuff[8] = (UTF8Char)(nanosec + 0x30);
				sbuff += 9;
				pattern += 9;
				while (*pattern == 'f')
				{
					*sbuff++ = '0';
					pattern++;
				}
			}
			break;
		}
		case 'F':
		{
			UInt8 digiCnt;
			UInt16 thisMS;
			if (pattern[1] != 'F')
			{
				digiCnt = 1;
				thisMS = (UInt16)(nanosec / 100000000);
				pattern += 1;
			}
			else if (pattern[2] != 'F')
			{
				digiCnt = 2;
				thisMS = (UInt16)(nanosec / 10000000);
				pattern += 2;
			}
			else
			{
				digiCnt = 3;
				thisMS = (UInt16)(nanosec / 1000000);
				pattern += 3;
			}

			while ((thisMS % 10) == 0)
			{
				thisMS = thisMS / 10;
				if (--digiCnt <= 0)
					break;
			}
			sbuff += digiCnt;
			UnsafeArray<UTF8Char> src = sbuff;
			while (digiCnt-- > 0)
			{
				*--src = (UTF8Char)((thisMS % 10) + 0x30);
				thisMS = thisMS / 10;
			}
			break;
		}
		case 'h':
		{
			UInt8 thisH = tval->hour % 12;
			if (thisH == 0)
			{
				thisH = 12;
			}
			pattern++;
			if (*pattern != 'h')
			{
				if (thisH < 10)
				{
					*sbuff++ = (UTF8Char)(thisH + 0x30);
				}
				else
				{
					WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[thisH * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[thisH * 2]));
				sbuff += 2;
				pattern++;

//				while (*pattern == 'h')
//					pattern++;
			}
			break;
		}
		case 'H':
		{
			pattern++;
			if (*pattern != 'H')
			{
				if (tval->hour < 10)
				{
					*sbuff++ = (UTF8Char)(tval->hour + 0x30);
				}
				else
				{
					WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->hour * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->hour * 2]));
				sbuff += 2;

				pattern++;
//				while (*pattern == 'H')
//					pattern++;
			}
			break;
		}
		case 'M':
		{
			if (pattern[1] != 'M')
			{
				if (tval->month < 10)
				{
					*sbuff++ = (UTF8Char)(tval->month + 0x30);
				}
				else
				{
					WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->month * 2]));
					sbuff += 2;
				}
				pattern += 1;
			}
			else if (pattern[2] != 'M')
			{
				WriteNUInt16(&sbuff[0], ReadNUInt16(&MyString_StrDigit100U8[tval->month * 2]));
				sbuff += 2;
				pattern += 2;
			}
			else if (pattern[3] != 'M')
			{
				WriteNUInt32(&sbuff[0], ReadNUInt32((const UInt8*)monString[tval->month - 1]));
				sbuff += 3;
				pattern += 3;
			}
			else
			{
				switch (tval->month)
				{
				case 1:
					*sbuff++ = 'J';
					*sbuff++ = 'a';
					*sbuff++ = 'n';
					*sbuff++ = 'u';
					*sbuff++ = 'a';
					*sbuff++ = 'r';
					*sbuff++ = 'y';
					break;
				case 2:
					*sbuff++ = 'F';
					*sbuff++ = 'e';
					*sbuff++ = 'b';
					*sbuff++ = 'r';
					*sbuff++ = 'u';
					*sbuff++ = 'a';
					*sbuff++ = 'r';
					*sbuff++ = 'y';
					break;
				case 3:
					*sbuff++ = 'M';
					*sbuff++ = 'a';
					*sbuff++ = 'r';
					*sbuff++ = 'c';
					*sbuff++ = 'h';
					break;
				case 4:
					*sbuff++ = 'A';
					*sbuff++ = 'p';
					*sbuff++ = 'r';
					*sbuff++ = 'i';
					*sbuff++ = 'l';
					break;
				case 5:
					*sbuff++ = 'M';
					*sbuff++ = 'a';
					*sbuff++ = 'y';
					break;
				case 6:
					*sbuff++ = 'J';
					*sbuff++ = 'u';
					*sbuff++ = 'n';
					*sbuff++ = 'e';
					break;
				case 7:
					*sbuff++ = 'J';
					*sbuff++ = 'u';
					*sbuff++ = 'l';
					*sbuff++ = 'y';
					break;
				case 8:
					*sbuff++ = 'A';
					*sbuff++ = 'u';
					*sbuff++ = 'g';
					*sbuff++ = 'u';
					*sbuff++ = 's';
					*sbuff++ = 't';
					break;
				case 9:
					*sbuff++ = 'S';
					*sbuff++ = 'e';
					*sbuff++ = 'p';
					*sbuff++ = 't';
					*sbuff++ = 'e';
					*sbuff++ = 'm';
					*sbuff++ = 'b';
					*sbuff++ = 'e';
					*sbuff++ = 'r';
					break;
				case 10:
					*sbuff++ = 'O';
					*sbuff++ = 'c';
					*sbuff++ = 't';
					*sbuff++ = 'o';
					*sbuff++ = 'b';
					*sbuff++ = 'e';
					*sbuff++ = 'r';
					break;
				case 11:
					*sbuff++ = 'N';
					*sbuff++ = 'o';
					*sbuff++ = 'v';
					*sbuff++ = 'e';
					*sbuff++ = 'm';
					*sbuff++ = 'b';
					*sbuff++ = 'e';
					*sbuff++ = 'r';
					break;
				case 12:
					*sbuff++ = 'D';
					*sbuff++ = 'e';
					*sbuff++ = 'c';
					*sbuff++ = 'e';
					*sbuff++ = 'm';
					*sbuff++ = 'b';
					*sbuff++ = 'e';
					*sbuff++ = 'r';
					break;
				}

				pattern += 4;
				while (*pattern == 'M')
					pattern++;
			}
			break;
		}
		case 't':
		{
			if (pattern[1] != 't')
			{
				if (tval->hour >= 12)
				{
					*sbuff++ = 'P';
				}
				else
				{
					*sbuff++ = 'A';
				}
				pattern += 1;
			}
			else
			{
				if (tval->hour >= 12)
				{
					sbuff[0] = 'P';
				}
				else
				{
					sbuff[0] = 'A';
				}
				sbuff[1] = 'M';
				sbuff += 2;
				pattern += 2;
				while (*pattern == 't')
					pattern++;
			}
			break;
		}
		case 'Z':
			if (tzQhr == 0)
			{
				sbuff[0] = 'Z';
				sbuff++;
				pattern++;
				while (*pattern == 'Z')
					pattern++;
				break;
			}
		case 'z':
		{
			Int32 hr = tzQhr >> 2;
			Int32 min = (tzQhr & 3) * 15;
			if (pattern[1] != 'z' && pattern[1] != 'Z')
			{
				if (hr >= 0)
				{
					sbuff[0] = '+';
				}
				else
				{
					sbuff[0] = '-';
					hr = -hr;
				}
				if (hr >= 10)
				{
					WriteNUInt16(&sbuff[1], ReadNUInt16(&MyString_StrDigit100U8[hr * 2]));
					sbuff += 3;
				}
				else
				{
					sbuff[1] = (UTF8Char)(hr + 0x30);
					sbuff += 2;
				}
				pattern++;
			}
			else if (pattern[2] != 'z' && pattern[2] != 'Z')
			{
				if (hr >= 0)
				{
					sbuff[0] = '+';
				}
				else
				{
					sbuff[0] = '-';
					hr = -hr;
				}
				WriteNUInt16(&sbuff[1], ReadNUInt16(&MyString_StrDigit100U8[hr * 2]));
				sbuff += 3;
				pattern += 2;
			}
			else if (pattern[3] != 'z' && pattern[3] != 'Z')
			{
				if (hr >= 0)
				{
					sbuff[0] = '+';
				}
				else
				{
					sbuff[0] = '-';
					hr = -hr;
				}
				WriteNUInt16(&sbuff[1], ReadNUInt16(&MyString_StrDigit100U8[hr * 2]));
				WriteNUInt16(&sbuff[3], ReadNUInt16(&MyString_StrDigit100U8[min * 2]));
				sbuff += 5;
				pattern += 3;
			}
			else
			{
				if (hr >= 0)
				{
					sbuff[0] = '+';
				}
				else
				{
					sbuff[0] = '-';
					hr = -hr;
				}
				WriteNUInt16(&sbuff[1], ReadNUInt16(&MyString_StrDigit100U8[hr * 2]));
				sbuff[3] = ':';
				WriteNUInt16(&sbuff[4], ReadNUInt16(&MyString_StrDigit100U8[min * 2]));
				sbuff += 6;
				pattern += 4;
				while (*pattern == 'z' || *pattern == 'Z')
					pattern++;
			}
			break;
		}
		case '\\':
		{
			if (pattern[1] == 0)
				*sbuff++ = (UTF8Char)*pattern++;
			else
			{
				pattern++;
				*sbuff++ = (UTF8Char)*pattern++;
			}
			break;
		}
		default:
			*sbuff++ = (UTF8Char)*pattern++;
			break;
		}
	}
	*sbuff = 0;
	return sbuff;
}

Bool Data::DateTimeUtil::String2TimeValue(Text::CStringNN dateStr, NN<TimeValue> tval, Int8 defTzQhr, OutParam<Int8> outTzQhr, OutParam<UInt32> nanosec)
{
	UTF8Char buff[64];
	UTF8Char *longBuff = 0;
	Text::PString strs2[5];
	Text::PString strs[3];
	UIntOS nStrs;
	Bool succ = true;
	outTzQhr.Set(defTzQhr);
	if (dateStr.v[3] == ',' && Text::StrIndexOfChar(&dateStr.v[4], ',') == INVALID_INDEX)
	{
		dateStr = dateStr.Substring(4);
		while (dateStr.v[0] == ' ')
			dateStr = dateStr.Substring(1);
	}
	if (dateStr.leng >= 64)
	{
		longBuff = MemAlloc(UTF8Char, dateStr.leng + 1);
		dateStr.ConcatTo(longBuff);
		strs[0] = {longBuff, dateStr.leng};
	}
	else
	{
		dateStr.ConcatTo(buff);
		strs[0] = {buff, dateStr.leng};
	}
	nStrs = Text::StrSplitTrimP(strs2, 5, strs[0], ' ');
	if (nStrs == 1)
	{
		nStrs = Text::StrSplitP(strs2, 3, strs[0], 'T');
	}
	if (nStrs == 2)
	{
		Bool dateSucc = true;
		if (Text::StrSplitP(strs, 3, strs2[0], '-') == 3)
		{
			if (!DateValueSetDate(tval, strs))
			{
				if (longBuff) MemFree(longBuff);
				return false;
			}
		}
		else if (Text::StrSplitP(strs, 3, strs2[0], '/') == 3)
		{
			if (!DateValueSetDate(tval, strs))
			{
				if (longBuff) MemFree(longBuff);
				return false;
			}
		}
		else if (Text::StrSplitP(strs, 3, strs2[0], ':') == 3)
		{
			if (!DateValueSetDate(tval, strs))
			{
				if (longBuff) MemFree(longBuff);
				return false;
			}
		}
		else
		{
			Secs2TimeValue(GetCurrTimeSecHighP(nanosec), tval, defTzQhr);
			dateSucc = false;
		}
		UIntOS i = strs2[1].IndexOf('-');
		if (i == INVALID_INDEX)
		{
			i = strs2[1].IndexOf('+');
		}
		if (i != INVALID_INDEX)
		{
			UTF8Char c = strs2[1].v[i];
			strs2[1].v[i] = 0;
			UIntOS tzlen = strs2[1].leng - i - 1;
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
					outTzQhr.Set((Int8)(-(Int32)min / 15));
				}
				else
				{
					outTzQhr.Set((Int8)(min / 15));
				}
			}
			else if (tzlen == 2)
			{
				if (c == '-')
				{
					outTzQhr.Set((Int8)-(Text::StrToInt32(&strs2[1].v[i + 1]) * 4));
				}
				else
				{
					outTzQhr.Set((Int8)(Text::StrToUInt32(&strs2[1].v[i + 1]) * 4));
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
				outTzQhr.Set(0);
			}
			TimeValueSetTime(tval, strs, nanosec);
		}
		else
		{
			tval->hour = 0;
			tval->minute = 0;
			tval->second = 0;
			nanosec.Set(0);
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
			if (!DateValueSetDate(tval, strs))
			{
				if (longBuff) MemFree(longBuff);
				return false;
			}
			tval->hour = 0;
			tval->minute = 0;
			tval->second = 0;
			nanosec.Set(0);
		}
		else if (Text::StrSplitP(strs, 3, strs2[0], '/') == 3)
		{
			if (!DateValueSetDate(tval, strs))
			{
				if (longBuff) MemFree(longBuff);
				return false;
			}
			tval->hour = 0;
			tval->minute = 0;
			tval->second = 0;
			nanosec.Set(0);
		}
		else if ((nStrs = Text::StrSplitP(strs, 3, strs2[0], ':')) == 3)
		{
			Secs2TimeValue(GetCurrTimeSecHighP(nanosec), tval, defTzQhr);
//			else
//				Ticks2TimeValue(GetCurrTimeMillis(), tval, *tzQhr);
			TimeValueSetTime(tval, strs, nanosec);
		}
		else if (nStrs == 2)
		{
			Secs2TimeValue(GetCurrTimeSecHighP(nanosec), tval, defTzQhr);
			if (!strs[0].ToUInt8(tval->hour))
				tval->hour = 0;
			if (!strs[1].ToUInt8(tval->minute))
				tval->minute = 0;
			tval->second = 0;
			nanosec.Set(0);
		}
		else
		{
			succ = false;
		}
	}
	else if (nStrs == 4 || (nStrs == 5 && (strs2[4].v[0] == '-' || strs2[4].v[0] == '+' || strs2[4].Equals(UTF8STRC("GMT")))))
	{
		UIntOS len1 = strs2[0].leng;
		UIntOS len2 = strs2[1].leng;
		UIntOS len3 = strs2[2].leng;
		UIntOS len4 = strs2[3].leng;
		UnsafeArray<UTF8Char> timeStr = strs2[3].v;
		UIntOS timeStrLen = strs2[3].leng;
		if (len1 == 3 && len2 <= 2 && len3 == 4)
		{
			tval->year = Data::DateTimeUtil::ParseYearStr(strs2[2].ToCString());
			tval->month = Data::DateTimeUtil::ParseMonthStr(strs2[0].ToCString());
			tval->day = Text::StrToUInt8(strs2[1].v);
		}
		else if (len1 <= 2 && len2 == 3 && len3 == 4)
		{
			tval->year = Data::DateTimeUtil::ParseYearStr(strs2[2].ToCString());
			tval->month = Data::DateTimeUtil::ParseMonthStr(strs2[1].ToCString());
			tval->day = Text::StrToUInt8(strs2[0].v);
		}
		else if (len1 == 3 && len2 <= 2 && len4 == 4)
		{
			tval->year = Data::DateTimeUtil::ParseYearStr(strs2[3].ToCString());
			tval->month = Data::DateTimeUtil::ParseMonthStr(strs2[0].ToCString());
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
			TimeValueSetTime(tval, strs, nanosec);
		}
		else
		{
			succ = false;
		}
		
		if (nStrs == 5)
		{
			if (strs2[4].Equals(UTF8STRC("GMT")))
			{
				outTzQhr.Set(0);
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
					outTzQhr.Set((Int8)(-min / 15));
				}
				else
				{
					outTzQhr.Set((Int8)(min / 15));
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
		outTzQhr.Set(0);

		UIntOS j = 0;
		UIntOS i;
		UIntOS splitCnt;
		while (true)
		{
			if ((splitCnt = Text::StrSplitP(strs, 3, strs2[j], ':')) == 3)
			{
				TimeValueSetTime(tval, strs, nanosec);
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
						outTzQhr.Set((Int8)(-(Int32)((Text::StrToUInt32(&strs[0].v[1]) << 2) + (Text::StrToUInt32(strs[1].v) / 15))));
					}
					else if (strs2[j].leng == 5)
					{
						Int8 tzQhr = (Int8)(Text::StrToUInt32(&strs2[j].v[3]) / 15);
						strs2[j].v[3] = 0;
						outTzQhr.Set((Int8)(-(tzQhr + (Int32)(Text::StrToUInt32(&strs2[j].v[1]) << 2))));
					}
				}
				else if (strs2[j].v[0] == '+')
				{
					if (Text::StrSplitP(strs, 3, strs2[j], ':') == 2)
					{
						outTzQhr.Set((Int8)((Text::StrToUInt32(&strs[0].v[1]) << 2) + (Text::StrToUInt32(strs[1].v) / 15)));
					}
					else if (strs2[j].leng == 5)
					{
						Int8 tzQhr = (Int8)(Text::StrToUInt32(&strs2[j].v[3]) / 15);
						strs2[j].v[3] = 0;
						outTzQhr.Set((Int8)(tzQhr + (Int32)(Text::StrToUInt32(&strs2[j].v[1]) << 2)));
					}
				}
				else
				{
					i = Text::StrIndexOfChar(strs2[j].v, '/');
					if (i != INVALID_INDEX && i > 0)
					{
						if (Text::StrSplitP(strs, 3, strs2[j], '/') == 3)
						{
							if (!DateValueSetDate(tval, strs))
							{
								if (longBuff) MemFree(longBuff);
								return false;
							}
						}
					}
					else if (Text::StrSplitP(strs, 3, strs2[j], '-') == 3)
					{
						if (!DateValueSetDate(tval, strs))
						{
							if (longBuff) MemFree(longBuff);
							return false;
						}
					}
					else if (strs2[j].Equals(UTF8STRC("HKT")))
					{
						outTzQhr.Set(32);
					}
					else
					{
						i = Text::StrToUInt32(strs2[j].v);
						if (i <= 0)
						{
							i = Data::DateTimeUtil::ParseMonthStr(strs2[j].ToCString());
							if (i > 0)
							{
								tval->month = (UInt8)i;
							}
						}
						else if (i > 100)
						{
							tval->year = (Int32)i;
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
	if (longBuff) MemFree(longBuff);
	return succ;
}

Bool Data::DateTimeUtil::TimeValueFromYMDHMS(Int64 ymdhms, NN<TimeValue> tval)
{
	if (ymdhms < 0)
		return false;
	tval->second = (UInt8)(ymdhms % 100);
	ymdhms = ymdhms / 100;
	if (tval->second >= 60)
		return false;
	tval->minute = (UInt8)(ymdhms % 100);
	ymdhms = ymdhms / 100;
	if (tval->minute >= 60)
		return false;
	tval->hour = (UInt8)(ymdhms % 100);
	ymdhms = ymdhms / 100;
	if (tval->hour >= 24)
		return false;
	tval->day = (UInt8)(ymdhms % 100);
	ymdhms = ymdhms / 100;
	if (tval->day == 0 || tval->day > 31)
		return false;
	tval->month = (UInt8)(ymdhms % 100);
	ymdhms = ymdhms / 100;
	if (tval->month == 0 || tval->month > 12)
		return false;
	if (ymdhms <= 0 || ymdhms > 65535)
		return false;
	tval->year = (UInt16)ymdhms;
	if (tval->day > Data::DateTimeUtil::DayInMonth(tval->year, tval->month))
		return false;
	return true;
}

Bool Data::DateTimeUtil::IsYearLeap(IntOS year)
{
	return ((year & 3) == 0) && ((year % 100) != 0 || (year % 400) == 0);
}

Int32 Data::DateTimeUtil::ParseYearStr(Text::CStringNN year)
{
	Int32 y;
	if (year.ToInt32(y))
	{
		if (y > 0)
			return y;
		return y + 1;
	}
	return 1970;
}

UInt8 Data::DateTimeUtil::ParseMonthStr(Text::CStringNN month)
{
	if (month.leng < 3)
		return 0;
	if (month.StartsWithICase(UTF8STRC("JAN")))
	{
		return 1;
	}
	else if (month.StartsWithICase(UTF8STRC("FEB")))
	{
		return 2;
	}
	else if (month.StartsWithICase(UTF8STRC("MAR")))
	{
		return 3;
	}
	else if (month.StartsWithICase(UTF8STRC("APR")))
	{
		return 4;
	}
	else if (month.StartsWithICase(UTF8STRC("MAY")))
	{
		return 5;
	}
	else if (month.StartsWithICase(UTF8STRC("JUN")))
	{
		return 6;
	}
	else if (month.StartsWithICase(UTF8STRC("JUL")))
	{
		return 7;
	}
	else if (month.StartsWithICase(UTF8STRC("AUG")))
	{
		return 8;
	}
	else if (month.StartsWithICase(UTF8STRC("SEP")))
	{
		return 9;
	}
	else if (month.StartsWithICase(UTF8STRC("OCT")))
	{
		return 10;
	}
	else if (month.StartsWithICase(UTF8STRC("NOV")))
	{
		return 11;
	}
	else if (month.StartsWithICase(UTF8STRC("DEC")))
	{
		return 12;
	}
	return 0;
}

UnsafeArray<UTF8Char> Data::DateTimeUtil::DispYear(UnsafeArray<UTF8Char> buff, Int32 year)
{
	return Text::StrInt32(buff, DispYearI32(year));
}

Int32 Data::DateTimeUtil::DispYearI32(Int32 year)
{
	if (year > 0)
		return year;
	else
		return year - 1;
}

Double Data::DateTimeUtil::MS2Days(Int64 ms)
{
	return (Double)ms / 86400000.0;
}

Double Data::DateTimeUtil::MS2Hours(Int64 ms)
{
	return (Double)ms / 3600000.0;
}

Double Data::DateTimeUtil::MS2Minutes(Int64 ms)
{
	return (Double)ms / 60000.0;
}

Double Data::DateTimeUtil::MS2Seconds(Int64 ms)
{
	return (Double)ms * 0.001;
}

UInt8 Data::DateTimeUtil::DayInMonth(Int32 year, UInt8 month)
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

Int8 Data::DateTimeUtil::GetLocalTzQhr()
{
	if (localTzValid)
		return localTzQhr;
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
	Int32 newTZ;
	time_t now = time(0);
	tm *t = localtime(&now);
	newTZ = (Int32)(t->tm_gmtoff / 900);
#else
	Int32 newTZ = 0;
#endif
	localTzQhr = (Int8)newTZ;
	localTzValid = true;
	return (Int8)newTZ;
}

Int64 Data::DateTimeUtil::GetCurrTimeMillis()
{
#if defined(_WIN32) || defined(_WIN32_WCE)
	TimeValue tval;
	SYSTEMTIME st;
	GetSystemTime(&st);
	tval.year = (Int32)st.wYear;
	tval.month = (UInt8)st.wMonth;
	tval.day = (UInt8)st.wDay;
	tval.hour = (UInt8)st.wHour;
	tval.minute = (UInt8)st.wMinute;
	tval.second = (UInt8)st.wSecond;
	return TimeValue2Ticks(tval, (UInt32)st.wMilliseconds * 1000000, 0);
#elif !defined(CPU_AVR)
	struct timeval tv;
	if (gettimeofday(&tv, 0) == 0)
	{
		return 1000 * (Int64)tv.tv_sec + tv.tv_usec / 1000;
	}
	else
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return 1000 * (Int64)ts.tv_sec + (ts.tv_nsec / 1000000);
	}
#else
	return 0;
#endif
}

Int64 Data::DateTimeUtil::GetCurrTimeSecHighP(OutParam<UInt32> nanosec)
{
#if defined(_WIN32_WCE)
	TimeValue tval;
	SYSTEMTIME st;
	GetSystemTime(&st);
	tval.year = (Int32)st.wYear;
	tval.month = (UInt8)st.wMonth;
	tval.day = (UInt8)st.wDay;
	tval.hour = (UInt8)st.wHour;
	tval.minute = (UInt8)st.wMinute;
	tval.second = (UInt8)st.wSecond;
	tval.ms = st.wMilliseconds;
	nanosec.Set((UInt32)st.wMilliseconds * 1000000);
	return TimeValue2Ticks(&tval, 0) / 1000LL;
#elif defined(_WIN32)
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	return FILETIME2Secs(&fileTime, nanosec);
#elif !defined(CPU_AVR)
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	nanosec.Set((UInt32)ts.tv_nsec);
	return (Int64)ts.tv_sec;
#else
	nanosec.Set(0);
	return 0;
#endif
}

Int64 Data::DateTimeUtil::FILETIME2Secs(const void *fileTime, OutParam<UInt32> nanosec)
{
	Int64 t = ReadInt64((const UInt8*)fileTime) - 116444736000000000LL;
	if (t < 0)
	{
		nanosec.Set((UInt32)(t % 10000000 + 10000000) * 100);
		return t / 10000000 - 1;
	}
	else
	{
		nanosec.Set((UInt32)(t % 10000000) * 100);
		return t / 10000000;
	}
}

void Data::DateTimeUtil::Secs2FILETIME(Int64 secs, UInt32 nanosec, void* fileTime)
{
	secs = secs * 10000000 + (Int64)(nanosec / 100);
	WriteInt64((UInt8*)fileTime, secs + 116444736000000000LL);
}

Int64 Data::DateTimeUtil::SYSTEMTIME2Ticks(const void *sysTime)
{
	const SYSTEMTIME *stime = (const SYSTEMTIME*)sysTime;
	TimeValue tval;
	tval.year = (Int32)stime->wYear;
	tval.month = (UInt8)stime->wMonth;
	tval.day = (UInt8)stime->wDay;
	tval.hour = (UInt8)stime->wHour;
	tval.minute = (UInt8)stime->wMinute;
	tval.second = (UInt8)stime->wSecond;
	return TimeValue2Ticks(tval, (UInt32)stime->wMilliseconds * 1000000, 0);
}

void Data::DateTimeUtil::Ticks2SYSTEMTIME(void *sysTime, Int64 ticks)
{
	Data::DateTimeUtil::TimeValue tval;
	Ticks2TimeValue(ticks, tval, 0);
	SYSTEMTIME *stime = (SYSTEMTIME*)sysTime;
	stime->wYear = (UInt16)tval.year;
	stime->wMonth = tval.month;
	stime->wDay = tval.day;
	stime->wHour = tval.hour;
	stime->wMinute = tval.minute;
	stime->wSecond = tval.second;
	if (ticks < 0)
		stime->wMilliseconds = (UInt16)(1000 + (ticks % 1000));
	else
		stime->wMilliseconds = (UInt16)(ticks % 1000);
	stime->wDayOfWeek = 0;
}

Bool Data::DateTimeUtil::SetAsComputerTime(Int64 secs, UInt32 nanosec)
{
#ifdef WIN32
	Data::DateTimeUtil::TimeValue tval;
	SYSTEMTIME st;
	Instant2TimeValue(secs, nanosec, tval, 0);
	st.wYear = (UInt16)tval.year;
	st.wMonth = tval.month;
	st.wDay = tval.day;
	st.wHour = tval.hour;
	st.wMinute = tval.minute;
	st.wSecond = tval.second;
	st.wMilliseconds = (UInt16)(nanosec / 1000000);
	return SetSystemTime(&st) != FALSE;
#elif !defined(CPU_AVR)
	struct timespec tp;
	tp.tv_sec = (time_t)secs;
	tp.tv_nsec = (long)nanosec;
	clock_settime(CLOCK_REALTIME, &tp);
	return clock_settime(CLOCK_REALTIME, &tp) == 0;
#else
	return false;
#endif
}

Data::DateTimeUtil::Weekday Data::DateTimeUtil::WeekdayParse(Text::CStringNN weekday)
{
	if (weekday.StartsWithICase(UTF8STRC("SUN")))
	{
		return Weekday::Sunday;
	}
	if (weekday.StartsWithICase(UTF8STRC("MON")))
	{
		return Weekday::Monday;
	}
	if (weekday.StartsWithICase(UTF8STRC("TUE")))
	{
		return Weekday::Tuesday;
	}
	if (weekday.StartsWithICase(UTF8STRC("WED")))
	{
		return Weekday::Wednesday;
	}
	if (weekday.StartsWithICase(UTF8STRC("THU")))
	{
		return Weekday::Thursday;
	}
	if (weekday.StartsWithICase(UTF8STRC("FRI")))
	{
		return Weekday::Friday;
	}
	if (weekday.StartsWithICase(UTF8STRC("SAT")))
	{
		return Weekday::Saturday;
	}
	static UInt8 jpSun[] = {0xE6, 0x97, 0xA5, 0x00};
	if (weekday.StartsWith(jpSun, 3))
	{
		return Weekday::Sunday;
	}
	static UInt8 jpMon[] = {0xE6, 0x9C, 0x88, 0x00};
	if (weekday.StartsWith(jpMon, 3))
	{
		return Weekday::Monday;
	}
	static UInt8 jpTue[] = {0xE7, 0x81, 0xAB, 0x00};
	if (weekday.StartsWith(jpTue, 3))
	{
		return Weekday::Tuesday;
	}
	static UInt8 jpWed[] = {0xE6, 0xB0, 0xB4, 0x00};
	if (weekday.StartsWith(jpWed, 3))
	{
		return Weekday::Wednesday;
	}
	static UInt8 jpThu[] = {0xE6, 0x9C, 0xA8, 0x00};
	if (weekday.StartsWith(jpThu, 3))
	{
		return Weekday::Thursday;
	}
	static UInt8 jpFri[] = {0xE9, 0x87, 0x91, 0x00};
	if (weekday.StartsWith(jpFri, 3))
	{
		return Weekday::Friday;
	}
	static UInt8 jpSat[] = {0xE5, 0x9C, 0x9F, 0x00};
	if (weekday.StartsWith(jpSat, 3))
	{
		return Weekday::Saturday;
	}
	static UInt8 chiSun[] = {0xE6, 0x97, 0xA5, 0x00};
	if (weekday.EndsWith(chiSun, 3))
	{
		return Weekday::Sunday;
	}
	static UInt8 chiMon[] = {0xE4, 0xB8, 0x80, 0x00};
	if (weekday.EndsWith(chiMon, 3))
	{
		return Weekday::Monday;
	}
	static UInt8 chiTue[] = {0xE4, 0xBA, 0x8C, 0x00};
	if (weekday.EndsWith(chiTue, 3))
	{
		return Weekday::Tuesday;
	}
	static UInt8 chiWed[] = {0xE4, 0xB8, 0x89, 0x00};
	if (weekday.EndsWith(chiWed, 3))
	{
		return Weekday::Wednesday;
	}
	static UInt8 chiThu[] = {0xE5, 0x9B, 0x9B, 0x00};
	if (weekday.EndsWith(chiThu, 3))
	{
		return Weekday::Thursday;
	}
	static UInt8 chiFri[] = {0xE4, 0xBA, 0x94, 0x00};
	if (weekday.EndsWith(chiFri, 3))
	{
		return Weekday::Friday;
	}
	static UInt8 chiSat[] = {0xE5, 0x85, 0xAD, 0x00};
	if (weekday.EndsWith(chiSat, 3))
	{
		return Weekday::Saturday;
	}
	return Weekday::Monday;
}

Text::CStringNN Data::DateTimeUtil::WeekdayGetName(Weekday wd)
{
	switch (wd)
	{
	case Weekday::Sunday:
		return CSTR("Sunday");
	case Weekday::Monday:
		return CSTR("Monday");
	case Weekday::Tuesday:
		return CSTR("Tuesday");
	case Weekday::Wednesday:
		return CSTR("Wednesday");
	case Weekday::Thursday:
		return CSTR("Thursday");
	case Weekday::Friday:
		return CSTR("Friday");
	case Weekday::Saturday:
		return CSTR("Saturday");
	default:
		return CSTR("Unknown");
	}
}
