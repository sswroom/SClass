#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/DateTimeUtil.h"
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

Int8 Data::DateTimeUtil::localTzQhr = 0;
Bool Data::DateTimeUtil::localTzValid = false;

Int64 Data::DateTimeUtil::TimeValue2Ticks(TimeValue *t, Int8 tzQhr)
{
	Int32 totalDays;
	Int32 leapDays;
	Int32 yearDiff;
	Int32 yearDiff100;
	Int32 yearDiff400;

	Int32 currYear = t->year;
	Int32 currMonth = t->month;
	Int32 currDay = t->day;

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
		if (IsYearLeap(t->year))
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

	return totalDays * 86400000LL + (t->ms + t->second * 1000 + t->minute * 60000 + t->hour * 3600000 - tzQhr * 900000);
}

void Data::DateTimeUtil::Ticks2TimeValue(Int64 ticks, TimeValue *t, Int8 tzQhr)
{
	ticks = ticks + tzQhr * 900000;
	Int32 totalDays = (Int32)(ticks / 86400000LL);
	UInt32 minutes;
	if (ticks < 0)
	{
		ticks -= totalDays * 86400000LL;
		while (ticks < 0)
		{
			totalDays -= 1;
			ticks += 86400000LL;
		}
		minutes = (UInt32)(ticks % 86400000LL);
	}
	else
	{
		minutes = (UInt32)(ticks % 86400000LL);
	}

	t->ms = (UInt16)(minutes % 1000);
	minutes = minutes / 1000;
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
			t->year = (UInt16)(2000 + ((totalDays / 1461) << 2));
			totalDays = totalDays % 1461;
			if (totalDays >= 366)
			{
				totalDays--;
				t->year = (UInt16)(t->year + totalDays / 365);
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

Data::DateTimeUtil::Weekday Data::DateTimeUtil::Ticks2Weekday(Int64 ticks, Int8 tzQhr)
{
	return (Data::DateTimeUtil::Weekday)(((ticks + tzQhr * 900000) / 86400000 + 4) % 7);
}

UTF8Char *Data::DateTimeUtil::ToString(UTF8Char *sbuff, const TimeValue *tval, Int8 tzQhr, const UTF8Char *pattern)
{
	while (*pattern)
	{
		switch (*pattern)
		{
		case 'y':
		{
			UInt16 thisVal = tval->year;
			UInt8 digiCnt = 1;
			pattern++;
			while (*pattern == 'y')
			{
				digiCnt++;
				pattern++;
			}
			sbuff += digiCnt;
			UTF8Char *src = sbuff;
			while (digiCnt >= 2)
			{
				src -= 2;
				WriteNUInt16(src, ReadNUInt16(&MyString_StrDigit100U8[(thisVal % 100) * 2]));
				thisVal = thisVal / 100;
				digiCnt = (UInt8)(digiCnt - 2);
			}
			if (digiCnt > 0)
			{
				*--src = (UTF8Char)((thisVal % 10) + 0x30);
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
					WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->minute * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->minute * 2]));
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
					WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->second * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->second * 2]));
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
					WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->day * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->day * 2]));
				sbuff += 2;

				pattern++;
//				while (*pattern == 'd')
//					pattern++;
			}
			break;
		}
		case 'f':
		{
			if (pattern[1] != 'f')
			{
				*sbuff = (UTF8Char)((tval->ms / 100) + 0x30);
				pattern += 1;
				sbuff++;
			}
			else if (pattern[2] != 'f')
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[(tval->ms / 10) * 2]));
				sbuff += 2;
				pattern += 2;
			}
			else
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[(tval->ms / 10) * 2]));
				sbuff[2] = (UTF8Char)((tval->ms % 10) + 0x30);
				sbuff += 3;
				pattern += 3;
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
				thisMS = tval->ms / 100;
				pattern += 1;
			}
			else if (pattern[2] != 'F')
			{
				digiCnt = 2;
				thisMS = tval->ms / 10;
				pattern += 2;
			}
			else
			{
				digiCnt = 3;
				thisMS = tval->ms ;
				pattern += 3;
			}

			while ((thisMS % 10) == 0)
			{
				thisMS = thisMS / 10;
				if (--digiCnt <= 0)
					break;
			}
			sbuff += digiCnt;
			UTF8Char *src = sbuff;
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
			pattern++;
			if (*pattern != 'h')
			{
				if (thisH < 10)
				{
					*sbuff++ = (UTF8Char)(thisH + 0x30);
				}
				else
				{
					WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[thisH * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[thisH * 2]));
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
					WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->hour * 2]));
					sbuff += 2;
				}
			}
			else
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->hour * 2]));
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
					WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->month * 2]));
					sbuff += 2;
				}
				pattern += 1;
			}
			else if (pattern[2] != 'M')
			{
				WriteNUInt16(sbuff, ReadNUInt16(&MyString_StrDigit100U8[tval->month * 2]));
				sbuff += 2;
				pattern += 2;
			}
			else if (pattern[3] != 'M')
			{
				static const Char *monthStr3[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
				WriteNUInt32(sbuff, ReadNUInt32((const UInt8*)monthStr3[tval->month - 1]));
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
		case 'z':
		{
			Int32 hr = tzQhr >> 2;
			Int32 min = (tzQhr & 3) * 15;
			if (pattern[1] != 'z')
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
			else if (pattern[2] != 'z')
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
			else if (pattern[3] != 'z')
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
				while (*pattern == 'z')
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

Bool Data::DateTimeUtil::IsYearLeap(UInt16 year)
{
	return ((year & 3) == 0) && ((year % 100) != 0 || (year % 400) == 0);
}

UInt8 Data::DateTimeUtil::ParseMonthStr(const UTF8Char *month, UOSInt monthLen)
{
	if (monthLen < 3)
		return 0;
	if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("JAN")))
	{
		return 1;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("FEB")))
	{
		return 2;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("MAR")))
	{
		return 3;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("APR")))
	{
		return 4;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("MAY")))
	{
		return 5;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("JUN")))
	{
		return 6;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("JUL")))
	{
		return 7;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("AUG")))
	{
		return 8;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("SEP")))
	{
		return 9;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("OCT")))
	{
		return 10;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("NOV")))
	{
		return 11;
	}
	else if (Text::StrStartsWithICaseC(month, monthLen, UTF8STRC("DEC")))
	{
		return 12;
	}
	return 0;
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

UInt8 Data::DateTimeUtil::DayInMonth(UInt16 year, UInt8 month)
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
	tval.year = st.wYear;
	tval.month = (UInt8)st.wMonth;
	tval.day = (UInt8)st.wDay;
	tval.hour = (UInt8)st.wHour;
	tval.minute = (UInt8)st.wMinute;
	tval.second = (UInt8)st.wSecond;
	tval.ms = st.wMilliseconds;
	return TimeValue2Ticks(&tval, 0);
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
