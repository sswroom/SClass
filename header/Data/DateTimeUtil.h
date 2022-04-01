#ifndef _SM_DATA_DATETIMEUTIL
#define _SM_DATA_DATETIMEUTIL

#define DATETIME_TICK_PER_SECOND 1000
#define DATETIME_TICK_PER_HOUR 3600000
#define DATETIME_TICK_PER_DAY 86400000

namespace Data
{
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

		struct TimeValue
		{
			UInt16 year;
			UInt8 month;
			UInt8 day;
			UInt8 hour;
			UInt8 minute;
			UInt8 second;
			UInt16 ms;
		};
	private:
		static Int8 localTzQhr;
		static Bool localTzValid;

	public:
		static Int64 TimeValue2Ticks(TimeValue *t, Int8 tzQr);
		static void Ticks2TimeValue(Int64 ticks, TimeValue *t, Int8 tzQhr);
		static Weekday Ticks2Weekday(Int64 ticks, Int8 tzQhr);

		static Bool IsYearLeap(UInt16 year);
		static UInt8 ParseMonthStr(const UTF8Char *month, UOSInt monthLen);
		static Double MS2Days(Int64 ms);
		static Double MS2Hours(Int64 ms);
		static Double MS2Minutes(Int64 ms);
		static Double MS2Seconds(Int64 ms);
		static UInt8 DayInMonth(UInt16 year, UInt8 month);
		static Int8 GetLocalTzQhr();
		static Int64 GetCurrTimeMillis();
	};
}
#endif
