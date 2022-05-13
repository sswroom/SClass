#ifndef _SM_DATA_DATETIME
#define _SM_DATA_DATETIME
#include "Data/DateTimeUtil.h"
#include "Data/IComparable.h"
#include "Text/PString.h"

#define DATETIME_TICK_PER_SECOND 1000
#define DATETIME_TICK_PER_HOUR 3600000
#define DATETIME_TICK_PER_DAY 86400000

namespace Data
{
	class DateTime// : public Data::IComparable
	{
	private:
		enum class TimeType
		{
			None,
			Ticks,
			Time
		};
		
		TimeType timeType;
		Int8 tzQhr; //* 15 minutes
		union
		{
			Int64 ticks;
			Data::DateTimeUtil::TimeValue t;
		} val;

	private:
		Data::DateTimeUtil::TimeValue *GetTimeValue();

		static void SetDate(Data::DateTimeUtil::TimeValue *t, Text::PString *dateStrs);
		static void SetTime(Data::DateTimeUtil::TimeValue *t, Text::PString *timeStrs);
		void FixValues();
	public:
		DateTime();
		DateTime(Int64 ticks);
		DateTime(Int64 ticks, Int8 tzQhr);
		DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second);
		DateTime(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms);
		DateTime(Text::CString dateStr);
		DateTime(Data::DateTime *dt);
		~DateTime();

		Bool SetAsComputerTime();
		Data::DateTime *SetCurrTime();
		Data::DateTime *SetCurrTimeUTC();
		void SetValue(const DateTime *time);
		void SetValue(UInt16 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms, Int8 tzQhr);
		void SetValue(UInt16 year, OSInt month, OSInt day, OSInt hour, OSInt minute, OSInt second, OSInt ms);
		void SetValue(Int64 ticks, Int8 tzQhr);
		void SetValueNoFix(UInt16 year, UInt8 month, UInt8 day, UInt8 hour, UInt8 minute, UInt8 second, UInt16 ms, Int8 tzQhr);
		Bool SetValueSlow(const Char *dateStr);
		Bool SetValue(Text::CString dateStr);
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
		OSInt CompareTo(Data::DateTime *obj);
		Int32 DateCompare(Data::DateTime *dt);
		Bool IsSameDay(Data::DateTime *dt);

		void ToUTCTime();
		void ToLocalTime();
		void ConvertTimeZoneQHR(Int8 tzQhr);
		void SetTimeZoneQHR(Int8 tzQhr);
		Int8 GetTimeZoneQHR();
		Data::DateTimeUtil::Weekday GetWeekday();
	};
}
#endif
