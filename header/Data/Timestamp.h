#ifndef _SM_DATA_TIMESTAMP
#define _SM_DATA_TIMESTAMP
#include "Data/DateTimeUtil.h"

namespace Data
{
	class Timestamp
	{
	public:
		Int64 ticks;
		Int8 tzQhr;

	public:
		Timestamp()
		{
			this->ticks = 0;
			this->tzQhr = 0;
		}

		Timestamp(Int64 ticks, Int8 tzQhr)
		{
			this->ticks = ticks;
			this->tzQhr = tzQhr;
		};
		
		~Timestamp()
		{
		}

		Timestamp AddDay(OSInt val)
		{
			return Timestamp(this->ticks + val * 86400000LL, this->tzQhr);
		}

		Timestamp AddHour(OSInt val)
		{
			return Timestamp(this->ticks + val * 3600000LL, this->tzQhr);
		}

		Timestamp AddMinute(OSInt val)
		{
			return Timestamp(this->ticks + val * 60000LL, this->tzQhr);
		}

		Timestamp AddSecond(OSInt val)
		{
			return Timestamp(this->ticks + val * 1000LL, this->tzQhr);
		}

		Timestamp AddMS(OSInt val)
		{
			return Timestamp(this->ticks + val, this->tzQhr);
		}

		Timestamp ClearTime()
		{
			return Timestamp(this->ticks - this->ticks % 86400000LL, this->tzQhr);
		}

		Int64 GetMSPassedDate()
		{
			return this->ticks % 86400000LL;
		};

		Int64 DiffMS(Timestamp ts)
		{
			return this->ticks - ts.ticks;
		}

		Int64 ToTicks()
		{
			return this->ticks;
		}

		Int64 ToDotNetTicks()
		{
			return this->ticks * 10000LL + 621355968000000000LL;
		}

		Int64 ToUnixTimestamp()
		{
			return this->ticks / 1000LL;
		}

		static Timestamp FromDotNetTicks(Int64 ticks, Int8 tzQhr)
		{
			return Timestamp(ticks / 10000LL - 62135596800000LL, tzQhr);
		}

		static Timestamp FromUnixTimestamp(Int64 ts)
		{
			return Timestamp(ts * 1000LL, 0);
		}

/*		void SetMSDOSTime(UInt16 date, UInt16 time);
		UInt16 ToMSDOSDate();
		UInt16 ToMSDOSTime();
		void ToSYSTEMTIME(void *sysTime);
		void SetNTPTime(Int32 hiDword, Int32 loDword);
		Int64 ToNTPTime();*/

		Char *ToString(Char *buff)
		{
			return (Char*)ToString((UTF8Char*)buff);
		}

		Char *ToString(Char *buff, const Char *pattern)
		{
			return (Char*)ToString((UTF8Char*)buff, pattern);
		}

		UTF8Char *ToString(UTF8Char *buff)
		{
			return ToString(buff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
		}

		UTF8Char *ToString(UTF8Char *buff, const Char *pattern)
		{
			Data::DateTimeUtil::TimeValue tval;
			Data::DateTimeUtil::Ticks2TimeValue(this->ticks, &tval, this->tzQhr);
			return Data::DateTimeUtil::ToString(buff, &tval, this->tzQhr, (const UTF8Char*)pattern);
		}
		
		Bool operator==(Timestamp dt)
		{
			return this->ticks == dt.ticks;
		}

		Bool operator>(Timestamp dt)
		{
			return this->ticks > dt.ticks;
		}

		Bool operator<(Timestamp dt)
		{
			return this->ticks < dt.ticks;
		}
		
		/*UTF8Char *ToLocalStr(UTF8Char *buff);
		OSInt CompareTo(Data::DateTime *obj);
		Int32 DateCompare(Data::DateTime *dt);
		Bool IsSameDay(Data::DateTime *dt);*/

		Timestamp ToUTCTime()
		{
			return Timestamp(this->ticks, 0);
		}

		Timestamp ToLocalTime()
		{
			return Timestamp(this->ticks, Data::DateTimeUtil::GetLocalTzQhr());
		}

		Timestamp ConvertTimeZoneQHR(Int8 tzQhr)
		{
			return Timestamp(this->ticks, tzQhr);
		}

		Timestamp SetTimeZoneQHR(Int8 tzQhr)
		{
			if (this->tzQhr != tzQhr)
			{
				return Timestamp(this->ticks + (tzQhr - this->tzQhr) * (Int64)(15 * 60000), tzQhr);
			}
			else
			{
				return *this;
			}
		}

		Int8 GetTimeZoneQHR()
		{
			return this->tzQhr;
		}

		Data::DateTimeUtil::Weekday GetWeekday()
		{
			return Data::DateTimeUtil::Ticks2Weekday(this->ticks, this->tzQhr);
		}

		static Timestamp Now()
		{
			return Timestamp(Data::DateTimeUtil::GetCurrTimeMillis(), Data::DateTimeUtil::GetLocalTzQhr());
		}

		static Timestamp UtcNow()
		{
			return Timestamp(Data::DateTimeUtil::GetCurrTimeMillis(), 0);
		}
	};
}
#endif