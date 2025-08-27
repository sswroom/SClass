#ifndef _SM_DATA_DATETIMEUTIL
#define _SM_DATA_DATETIMEUTIL
#include "Text/CString.h"
#include "Text/PString.h"

#define DATETIME_TICK_PER_SECOND 1000
#define DATETIME_TICK_PER_HOUR 3600000
#define DATETIME_TICK_PER_DAY 86400000

namespace Data
{
	class TimeInstant;

	class DateTimeUtil
	{
	public:
		enum class Weekday
		{
			Sunday,
			Monday,
			Tuesday,
			Wednesday,
			Thursday,
			Friday,
			Saturday
		};

		struct DateValue
		{
			Int32 year;
			UInt8 month;
			UInt8 day;
		};

		struct TimeValue : public DateValue
		{
			UInt8 hour;
			UInt8 minute;
			UInt8 second;
		};

		static const Char *monString[]; //0-11
		static const Char *monthString[]; //0-11
	private:
		static Int8 localTzQhr;
		static Bool localTzValid;

		static Bool DateValueSetDate(NN<Data::DateTimeUtil::DateValue> t, Text::PString *dateStrs);
		static Bool TimeValueSetTime(NN<Data::DateTimeUtil::TimeValue> t, Text::PString *timeStrs, OutParam<UInt32> nanosec);
	public:
		static Int64 Date2TotalDays(Int32 year, Int32 month, Int32 day);
		static Int64 DateValue2TotalDays(NN<const DateValue> d);
		static Int64 TimeValue2Secs(NN<const TimeValue> t, Int8 tzQhr);
		static Int64 TimeValue2Ticks(NN<const TimeValue> t, UInt32 ns, Int8 tzQhr);
		static void Ticks2TimeValue(Int64 ticks, NN<TimeValue> t, Int8 tzQhr);
		static void Secs2TimeValue(Int64 secs, NN<TimeValue> t, Int8 tzQhr);
		static void TotalDays2DateValue(Int64 totalDays, NN<DateValue> d);
		static void Instant2TimeValue(Int64 secs, UInt32 nanosec, NN<TimeValue> t, Int8 tzQhr);
		static Weekday Ticks2Weekday(Int64 ticks, Int8 tzQhr);
		static Weekday Instant2Weekday(Data::TimeInstant inst, Int8 tzQhr);
		static UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> sbuff, NN<const TimeValue> tval, Int8 tzQhr, UInt32 nanosec, UnsafeArray<const UTF8Char> pattern);
		static Bool String2TimeValue(Text::CStringNN dateStr, NN<TimeValue> tval, Int8 defTzQhr, OutParam<Int8> outTzQhr, OutParam<UInt32> nanosec);
		static Bool TimeValueFromYMDHMS(Int64 ymdhms, NN<TimeValue> tval);

		static Bool IsYearLeap(OSInt year);
		static Int32 ParseYearStr(Text::CStringNN year);
		static UInt8 ParseMonthStr(Text::CStringNN month);
		static UnsafeArray<UTF8Char> DispYear(UnsafeArray<UTF8Char> buff, Int32 year);
		static Int32 DispYearI32(Int32 year);
		static Double MS2Days(Int64 ms);
		static Double MS2Hours(Int64 ms);
		static Double MS2Minutes(Int64 ms);
		static Double MS2Seconds(Int64 ms);
		static UInt8 DayInMonth(Int32 year, UInt8 month);
		static Int8 GetLocalTzQhr();
		static Int64 GetCurrTimeMillis();
		static Int64 GetCurrTimeSecHighP(OutParam<UInt32> nanosec);
		static Int64 FILETIME2Secs(const void *fileTime, OutParam<UInt32> nanosec);
		static void Secs2FILETIME(Int64 secs, UInt32 nanosec, void *fileTime);
		static Int64 SYSTEMTIME2Ticks(const void *sysTime);
		static void Ticks2SYSTEMTIME(void *sysTime, Int64 ticks);
		static Bool SetAsComputerTime(Int64 secs, UInt32 nanosec);
		static Weekday WeekdayParse(Text::CStringNN weekday);
		static Text::CStringNN WeekdayGetName(Weekday wd);
	};
}
#endif
