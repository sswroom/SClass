#ifndef _SM_DATA_DATETIME
#define _SM_DATA_DATETIME
#include "Data/IComparable.h"

#define DATETIME_TICK_PER_SECOND 1000
#define DATETIME_TICK_PER_HOUR 3600000
#define DATETIME_TICK_PER_DAY 86400000

namespace Data
{
	class DateTime : public Data::IComparable
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

		enum class TimeType
		{
			None,
			Ticks,
			Time
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
		TimeType timeType;
		Int8 tzQhr; //* 15 minutes
		union
		{
			Int64 ticks;
			TimeValue t;
		} val;

	private:
		TimeValue *GetTimeValue();

		void SetDate(Char **dateStrs);
		void SetTime(Char **timeStrs);
		void FixValues();
	public:
		DateTime();
		DateTime(Int64 ticks);
		DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second);
		DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms);
		DateTime(const UTF8Char *dateStr);
		DateTime(Data::DateTime *dt);
		virtual ~DateTime();

		Bool SetAsComputerTime();
		Data::DateTime *SetCurrTime();
		Data::DateTime *SetCurrTimeUTC();
		void SetValue(const DateTime *time);
		void SetValue(UInt16 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms, Int8 tzQhr);
		void SetValue(UInt16 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms);
		Bool SetValue(const Char *dateStr);
		Bool SetValue(const UTF8Char *dateStr);
		void SetValueSYSTEMTIME(void *sysTime);
		void SetValueFILETIME(void *fileTime);
		void SetValueVariTime(Double variTime);

		UInt16 GetYear();
		UInt8 GetMonth();
		UInt8 GetDay();
		UInt8 GetHour();
		UInt8 GetMinute();
		UInt8 GetSecond();
		UInt16 GetMS();
		Data::DateTime *AddMonth(OSInt val);
		Data::DateTime *AddDay(OSInt val);
		Data::DateTime *AddHour(OSInt val);
		Data::DateTime *AddMinute(OSInt val);
		Data::DateTime *AddSecond(OSInt val);
		Data::DateTime *AddMS(OSInt val);
		void SetDate(UInt16 year, OSInt month, OSInt day);
		void SetYear(UInt16 year);
		void SetMonth(OSInt month);
		void SetDay(OSInt day);
		void SetHour(OSInt hour);
		void SetMinute(OSInt minute);
		void SetSecond(OSInt second);
		void SetMS(OSInt ms);
		void ClearTime();
		Int64 GetMSPassedDate();

		Int64 DiffMS(DateTime *dt);

		Bool IsYearLeap();
		Int64 ToTicks();
		Int64 ToDotNetTicks();
		Int64 ToUnixTimestamp();
		void SetTicks(Int64 ticks);
		void SetDotNetTicks(Int64 ticks);
		void SetUnixTimestamp(Int64 ticks);
		void SetMSDOSTime(UInt16 date, UInt16 time);
		UInt16 ToMSDOSDate();
		UInt16 ToMSDOSTime();
		void ToSYSTEMTIME(void *sysTime);
		void SetNTPTime(Int32 hiDword, Int32 loDword);
		Int64 ToNTPTime();

		Char *ToString(Char *buff);
		Char *ToString(Char *buff, const Char *pattern);
		UTF8Char *ToString(UTF8Char *buff);
		UTF8Char *ToString(UTF8Char *buff, const Char *pattern);
		DateTime operator=(DateTime dt);
		DateTime operator=(Int64 ticks);
		Bool operator==(DateTime dt);
		Bool operator>(DateTime dt);
		Bool operator<(DateTime dt);
		
		UTF8Char *ToLocalStr(UTF8Char *buff);
		virtual OSInt CompareTo(Data::IComparable *obj);
		Int32 DateCompare(Data::DateTime *dt);
		Bool IsSameDay(Data::DateTime *dt);

		void ToUTCTime();
		void ToLocalTime();
		void ConvertTimeZoneQHR(Int8 tzQhr);
		void SetTimeZoneQHR(Int8 tzQhr);
		Int8 GetTimeZoneQHR();
		Weekday GetWeekday();

		static Int64 TimeValue2Ticks(TimeValue *t, Int8 tzQr);
		static void Ticks2TimeValue(Int64 ticks, TimeValue *t, Int8 tzQr);

		static Bool IsYearLeap(UInt16 year);
		static UInt8 ParseMonthStr(const Char *month);
		static UInt8 ParseMonthStr(const UTF8Char *month);
		static Double MS2Days(Int64 ms);
		static Double MS2Hours(Int64 ms);
		static Double MS2Minutes(Int64 ms);
		static Double MS2Seconds(Int64 ms);
		static UInt8 DayInMonth(UInt16 year, UInt8 month);
		static Int8 GetLocalTzQhr();
	};
}
#endif
