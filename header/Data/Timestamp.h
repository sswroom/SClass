#ifndef _SM_DATA_TIMESTAMP
#define _SM_DATA_TIMESTAMP
#include "Data/DateTimeUtil.h"

namespace Data
{
	class Timestamp
	{
	public:
		Int64 ticks;
		UInt32 nanosec;
		Int8 tzQhr;

	public:
		Timestamp() = default;

		Timestamp(Int64 ticks, Int8 tzQhr)
		{
			this->ticks = ticks;
			this->nanosec = (UInt32)(ticks % 1000) * 1000000;
			this->tzQhr = tzQhr;
		};
		
		Timestamp(Int64 ticks, UInt32 nanosec, Int8 tzQhr)
		{
			this->ticks = ticks;
			this->nanosec = nanosec;
			this->tzQhr = tzQhr;
		};
		
		~Timestamp()
		{
		}

		Timestamp AddDay(OSInt val) const
		{
			return Timestamp(this->ticks + val * 86400000LL, this->tzQhr);
		}

		Timestamp AddHour(OSInt val) const
		{
			return Timestamp(this->ticks + val * 3600000LL, this->tzQhr);
		}

		Timestamp AddMinute(OSInt val) const
		{
			return Timestamp(this->ticks + val * 60000LL, this->tzQhr);
		}

		Timestamp AddSecond(OSInt val) const
		{
			return Timestamp(this->ticks + val * 1000LL, this->tzQhr);
		}

		Timestamp AddMS(OSInt val) const
		{
			Int64 newTick = this->ticks + val;
			return Timestamp(newTick, (nanosec % 1000000) + (UInt32)(newTick % 1000) * 1000000, this->tzQhr);
		}

		Timestamp ClearTime() const
		{
			return Timestamp(this->ticks - this->ticks % 86400000LL, this->tzQhr);
		}

		Int64 GetMSPassedDate() const
		{
			return this->ticks % 86400000LL;
		};

		Int64 DiffMS(Timestamp ts) const
		{
			return this->ticks - ts.ticks;
		}

		Int64 ToTicks() const
		{
			return this->ticks;
		}

		Int64 ToDotNetTicks() const
		{
			return this->ticks * 10000LL + 621355968000000000LL;
		}

		Int64 ToUnixTimestamp() const
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

		Char *ToString(Char *buff) const
		{
			return (Char*)ToString((UTF8Char*)buff);
		}

		Char *ToString(Char *buff, const Char *pattern) const
		{
			return (Char*)ToString((UTF8Char*)buff, pattern);
		}

		UTF8Char *ToString(UTF8Char *buff) const
		{
			return ToString(buff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
		}

		UTF8Char *ToString(UTF8Char *buff, const Char *pattern) const
		{
			Data::DateTimeUtil::TimeValue tval;
			Data::DateTimeUtil::Ticks2TimeValue(this->ticks, &tval, this->tzQhr);
			return Data::DateTimeUtil::ToString(buff, &tval, this->tzQhr, this->nanosec, (const UTF8Char*)pattern);
		}
		
		Bool operator==(Timestamp dt) const
		{
			return this->ticks == dt.ticks;
		}

		Bool operator>(Timestamp dt) const
		{
			return this->ticks > dt.ticks;
		}

		Bool operator<(Timestamp dt) const
		{
			return this->ticks < dt.ticks;
		}

		OSInt CompareTo(const Timestamp& ts) const
		{
			if (this->ticks > ts.ticks)
				return 1;
			else if (this->ticks < ts.ticks)
				return -1;
			else
				return 0;
		}
		
		/*UTF8Char *ToLocalStr(UTF8Char *buff);
		OSInt CompareTo(Data::DateTime *obj);
		Int32 DateCompare(Data::DateTime *dt);
		Bool IsSameDay(Data::DateTime *dt);*/

		Timestamp ToUTCTime() const
		{
			return Timestamp(this->ticks, 0);
		}

		Timestamp ToLocalTime() const
		{
			return Timestamp(this->ticks, Data::DateTimeUtil::GetLocalTzQhr());
		}

		Timestamp ConvertTimeZoneQHR(Int8 tzQhr) const
		{
			return Timestamp(this->ticks, tzQhr);
		}

		Timestamp SetTimeZoneQHR(Int8 tzQhr) const
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

		Int8 GetTimeZoneQHR() const
		{
			return this->tzQhr;
		}

		Data::DateTimeUtil::Weekday GetWeekday() const
		{
			return Data::DateTimeUtil::Ticks2Weekday(this->ticks, this->tzQhr);
		}

		Bool SameDate(Data::Timestamp ts) const
		{
			return (this->ticks / 86400000) == (ts.ticks / 86400000);
		}

		static Timestamp Now()
		{
			UInt32 nanosec;
			Int64 ticks = Data::DateTimeUtil::GetCurrTimeHighP(&nanosec);
			return Timestamp(ticks, nanosec, Data::DateTimeUtil::GetLocalTzQhr());
		}

		static Timestamp UtcNow()
		{
			UInt32 nanosec;
			Int64 ticks = Data::DateTimeUtil::GetCurrTimeHighP(&nanosec);
			return Timestamp(ticks, nanosec, 0);
		}
	};
}
#endif
