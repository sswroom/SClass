#ifndef _SM_DATA_TIMESTAMP
#define _SM_DATA_TIMESTAMP
#include "Data/DateTimeUtil.h"
#include "Data/TimeInstant.h"

namespace Data
{
	class Timestamp
	{
	public:
		Data::TimeInstant inst;
		Int8 tzQhr;

	public:
		Timestamp() = default;

		Timestamp(UInt32 *tmp)
		{
			this->inst = Data::TimeInstant(0, 0);
			this->tzQhr = 127;
		}

		Timestamp(Int64 ticks, Int8 tzQhr)
		{
			this->inst = Data::TimeInstant::FromTicks(ticks);
			this->tzQhr = tzQhr;
		};

		Timestamp(Data::TimeInstant inst, Int8 tzQhr)
		{
			this->inst = inst;
			this->tzQhr = tzQhr;
		};

		Timestamp(Text::CString dateStr, Int8 defTzQhr)
		{
			Data::DateTimeUtil::TimeValue tval;
			this->tzQhr = defTzQhr;
			UInt32 nanosec = 0;
			if (!Data::DateTimeUtil::String2TimeValue(dateStr, &tval, &this->tzQhr, &nanosec))
			{
				this->inst = Data::TimeInstant(0, 0);
			}
			else
			{
				this->inst = Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(&tval, this->tzQhr), nanosec);
			}
		}
		
		~Timestamp() = default;

		Timestamp AddMonth(OSInt val) const
		{
			Data::DateTimeUtil::TimeValue tval;
			this->ToTimeValue(&tval);
			val += tval.month;
			while (val < 1)
			{
				val += 12;
				tval.year--;
			}
			while (val > 12)
			{
				val -= 12;
				tval.year++;
			}
			tval.month = (UInt8)val;
			return Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(&tval, this->tzQhr), this->inst.nanosec), this->tzQhr);
		}

		Timestamp AddYear(OSInt val) const
		{
			Data::DateTimeUtil::TimeValue tval;
			this->ToTimeValue(&tval);
			tval.year = (UInt16)(tval.year + val);
			return Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(&tval, this->tzQhr), this->inst.nanosec), this->tzQhr);
		}

		Timestamp AddDay(OSInt val) const
		{
			return Timestamp(this->inst.AddDay(val), this->tzQhr);
		}

		Timestamp AddHour(OSInt val) const
		{
			return Timestamp(this->inst.AddHour(val), this->tzQhr);
		}

		Timestamp AddMinute(OSInt val) const
		{
			return Timestamp(this->inst.AddMinute(val), this->tzQhr);
		}

		Timestamp AddSecond(OSInt val) const
		{
			return Timestamp(this->inst.AddSecond(val), this->tzQhr);
		}

		Timestamp AddSecondDbl(Double val) const
		{
			Int64 sec = (Int64)val;
			Int64 ns = (Int64)((val - (Double)sec) * 1000000000);
			return Timestamp(this->inst.AddSecond(sec).AddNS(ns), this->tzQhr);
		}

		Timestamp AddMS(OSInt val) const
		{
			return Timestamp(this->inst.AddMS(val), this->tzQhr);
		}

		Timestamp AddNS(Int64 val) const
		{
			return Timestamp(this->inst.AddNS(val), this->tzQhr);
		}

		UInt32 GetMS() const
		{
			return this->inst.GetMS();
		}

		Timestamp ClearTimeUTC() const
		{
			return Timestamp(this->inst.ClearTime(), this->tzQhr);
		}

		Timestamp ClearTimeLocal() const
		{
			return Timestamp(this->inst.AddMinute((OSInt)this->tzQhr * 15).ClearTime().AddMinute((OSInt)this->tzQhr * -15), this->tzQhr);
		}

		Timestamp ClearMonthAndDay() const
		{
			Data::DateTimeUtil::TimeValue tval;
			this->ToTimeValue(&tval);
			tval.month = 1;
			tval.day = 1;
			tval.hour = 0;
			tval.minute = 0;
			tval.second = 0;
			return Data::Timestamp::FromTimeValue(&tval, 0, this->tzQhr);
		}

		Timestamp ClearDayOfMonth() const
		{
			Data::DateTimeUtil::TimeValue tval;
			this->ToTimeValue(&tval);
			tval.day = 1;
			tval.hour = 0;
			tval.minute = 0;
			tval.second = 0;
			return Data::Timestamp::FromTimeValue(&tval, 0, this->tzQhr);
		}

		Int64 GetMSPassedUTCDate() const
		{
			return this->inst.GetMSPassedDate();
		};

		Int64 GetMSPassedLocalDate() const
		{
			return this->inst.AddSecond((OSInt)this->tzQhr * 900).GetMSPassedDate();
		};

		Int64 DiffSec(const Timestamp &ts) const
		{
			return this->inst.DiffSec(ts.inst);
		}

		Int64 DiffMS(const Timestamp &ts) const
		{
			return this->inst.DiffMS(ts.inst);
		}

		Double DiffSecDbl(const Timestamp &ts) const
		{
			return this->inst.DiffSecDbl(ts.inst);
		}

		Data::Duration Diff(const Timestamp &ts) const
		{
			return this->inst.Diff(ts.inst);
		}

		Int64 ToTicks() const
		{
			return this->inst.ToTicks();
		}

		Int64 ToDotNetTicks() const
		{
			return this->inst.ToDotNetTicks();
		}

		Int64 ToUnixTimestamp() const
		{
			return this->inst.ToUnixTimestamp();
		}

		Int64 ToEpochSec() const
		{
			return this->inst.ToEpochSec();
		}

		Int64 ToEpochMS() const
		{
			return this->inst.ToEpochMS();
		}

		Int64 ToEpochNS() const
		{
			return this->inst.ToEpochNS();
		}

/*		void SetMSDOSTime(UInt16 date, UInt16 time);
		UInt16 ToMSDOSDate();
		UInt16 ToMSDOSTime();*/

		void ToSYSTEMTIME(void *sysTime) const
		{
			Data::DateTimeUtil::Ticks2SYSTEMTIME(sysTime, this->ToTicks());
		}

		void ToFILETIME(void* fileTime) const
		{
			this->inst.ToFILETIME(fileTime);
		}
		
/*		void SetNTPTime(Int32 hiDword, Int32 loDword);
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
			if (this->IsNull())
			{
				*buff = 0;
				return buff;
			}
			else if (this->inst.nanosec == 0)
			{
				return ToString(buff, "yyyy-MM-dd HH:mm:ss zzzz");
			}
			else if (this->inst.nanosec % 1000000 == 0)
			{
				return ToString(buff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			}
			else if (this->inst.nanosec % 1000 == 0)
			{
				return ToString(buff, "yyyy-MM-dd HH:mm:ss.ffffff zzzz");
			}
			else
			{
				return ToString(buff, "yyyy-MM-dd HH:mm:ss.fffffffff zzzz");
			}
		}

		UTF8Char *ToStringNoZone(UTF8Char *buff) const
		{
			if (this->IsNull())
			{
				*buff = 0;
				return buff;
			}
			else if (this->inst.nanosec == 0)
			{
				if (((this->inst.sec + (OSInt)this->tzQhr * 900) % 86400) == 0)
				{
					return ToString(buff, "yyyy-MM-dd");
				}
				else
				{
					return ToString(buff, "yyyy-MM-dd HH:mm:ss");
				}
			}
			else if (this->inst.nanosec % 1000000 == 0)
			{
				return ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			else if (this->inst.nanosec % 1000 == 0)
			{
				return ToString(buff, "yyyy-MM-dd HH:mm:ss.ffffff");
			}
			else
			{
				return ToString(buff, "yyyy-MM-dd HH:mm:ss.fffffffff");
			}
		}

		UTF8Char *ToString(UTF8Char *buff, const Char *pattern) const
		{
			Data::DateTimeUtil::TimeValue tval;
			Data::DateTimeUtil::Instant2TimeValue(this->inst.sec, this->inst.nanosec, &tval, this->tzQhr);
			return Data::DateTimeUtil::ToString(buff, &tval, this->tzQhr, this->inst.nanosec, (const UTF8Char*)pattern);
		}
		
		Bool operator==(Timestamp dt) const
		{
			return this->inst == dt.inst;
		}

		Bool operator!=(Timestamp dt) const
		{
			return this->inst != dt.inst;
		}

		Bool operator>=(Timestamp dt) const
		{
			return this->inst >= dt.inst;
		}

		Bool operator<=(Timestamp dt) const
		{
			return this->inst <= dt.inst;
		}

		Bool operator>(Timestamp dt) const
		{
			return this->inst > dt.inst;
		}

		Bool operator<(Timestamp dt) const
		{
			return this->inst < dt.inst;
		}

		OSInt CompareTo(const Timestamp& ts) const
		{
			return this->inst.CompareTo(ts.inst);
		}
		
		/*UTF8Char *ToLocalStr(UTF8Char *buff);
		OSInt CompareTo(Data::DateTime *obj);
		Int32 DateCompare(Data::DateTime *dt);
		Bool IsSameDay(Data::DateTime *dt);*/

		Timestamp ToUTCTime() const
		{
			return Timestamp(this->inst, 0);
		}

		Timestamp ToLocalTime() const
		{
			return Timestamp(this->inst, Data::DateTimeUtil::GetLocalTzQhr());
		}

		Timestamp ConvertTimeZoneQHR(Int8 tzQhr) const
		{
			return Timestamp(this->inst, tzQhr);
		}

		Timestamp SetTimeZoneQHR(Int8 tzQhr) const
		{
			if (this->tzQhr != tzQhr)
			{
				return Timestamp(this->inst.AddSecond(((OSInt)this->tzQhr - tzQhr) * (OSInt)(15 * 60)), tzQhr);
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

		Data::DateTimeUtil::TimeValue GetTimeValue() const
		{
			Data::DateTimeUtil::TimeValue tv;
			Data::DateTimeUtil::Ticks2TimeValue(this->inst.ToTicks(), &tv, this->tzQhr);
			return tv;
		}

		Data::DateTimeUtil::Weekday GetWeekday() const
		{
			return Data::DateTimeUtil::Instant2Weekday(this->inst, this->tzQhr);
		}

		Bool SameDate(const Data::Timestamp &ts) const
		{
			return this->inst.SameDate(ts.inst);
		}

		Bool IsNull() const
		{
			return this->inst.sec == 0 && this->tzQhr == 127;
		}
		
		Bool SetAsComputerTime() const
		{
			return Data::DateTimeUtil::SetAsComputerTime(this->inst.sec, this->inst.nanosec);
		}

		void ToTimeValue(Data::DateTimeUtil::TimeValue *tval) const
		{
			Data::DateTimeUtil::Instant2TimeValue(this->inst.sec, this->inst.nanosec, tval, this->tzQhr);
		}

		Timestamp RoundToS() const
		{
			return Timestamp(this->inst.RoundToS(), this->tzQhr);
		}

		static Timestamp Now()
		{
			return Timestamp(TimeInstant::Now(), Data::DateTimeUtil::GetLocalTzQhr());
		}

		static Timestamp UtcNow()
		{
			return Timestamp(TimeInstant::Now(), 0);
		}

		static Timestamp FromVariTime(Double variTime)
		{
			return Data::Timestamp(TimeInstant::FromVariTime(variTime), Data::DateTimeUtil::GetLocalTzQhr());
		}

		static Timestamp FromStr(Text::CString s, Int8 defTZQhr)
		{
			Data::DateTimeUtil::TimeValue tv;
			Int8 tzQhr = defTZQhr;
			UInt32 nanosec;
			if (Data::DateTimeUtil::String2TimeValue(s, &tv, &tzQhr, &nanosec))
			{
				return Timestamp(TimeInstant(Data::DateTimeUtil::TimeValue2Secs(&tv, tzQhr), nanosec), tzQhr);
			}
			else
			{
				return Timestamp(0);
			}
		}

		static Timestamp FromSecNS(Int64 unixTS, UInt32 nanosec, Int8 tzQhr)
		{
			return Timestamp(Data::TimeInstant(unixTS, nanosec), tzQhr);
		}

		static Timestamp FromDotNetTicks(Int64 ticks, Int8 tzQhr)
		{
			return Timestamp(Data::TimeInstant::FromDotNetTicks(ticks), tzQhr);
		}

		static Timestamp FromEpochSec(Int64 epochSec, Int8 tzQhr)
		{
			return Timestamp(Data::TimeInstant(epochSec, 0), tzQhr);
		}

		static Timestamp FromEpochMS(Int64 epochMS, Int8 tzQhr)
		{
			return Timestamp(epochMS, tzQhr);
		}

		static Timestamp FromEpochUS(Int64 epochUS, Int8 tzQhr)
		{
			if (epochUS < 0)
			{
				return Timestamp(Data::TimeInstant(epochUS / 1000000 - 1, (UInt32)(epochUS % 1000000 + 1000000) * 1000), tzQhr);
			}
			else
			{
				return Timestamp(Data::TimeInstant(epochUS / 1000000, (UInt32)(epochUS % 1000000) * 1000), tzQhr);
			}
		}

		static Timestamp FromEpochNS(Int64 epochNS, Int8 tzQhr)
		{
			if (epochNS < 0)
			{
				return Timestamp(Data::TimeInstant(epochNS / 1000000000 - 1, (UInt32)(epochNS % 1000000000 + 1000000000)), tzQhr);
			}
			else
			{
				return Timestamp(Data::TimeInstant(epochNS / 1000000000, (UInt32)(epochNS % 1000000000)), tzQhr);
			}
		}

		static Timestamp FromTimeValue(const Data::DateTimeUtil::TimeValue *tval, UInt32 nanosec, Int8 tzQhr)
		{
			return Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(tval, tzQhr), nanosec), tzQhr);
		}

		static Timestamp FromFILETIME(void* filetime, Int8 tzQhr)
		{
			UInt32 nanosec;
			Int64 secs = Data::DateTimeUtil::FILETIME2Secs(filetime, &nanosec);
			return Data::Timestamp(Data::TimeInstant(secs, nanosec), tzQhr);
		}

		static Timestamp FromYMDHMS(Int64 ymdhms, Int8 tzQhr)
		{
			Data::DateTimeUtil::TimeValue tval;
			if (Data::DateTimeUtil::TimeValueFromYMDHMS(ymdhms, &tval))
			{
				return Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(&tval, tzQhr), 0), tzQhr);
			}
			else
			{
				return Null();
			}
		}

		static Timestamp Null()
		{
			return Data::Timestamp(0);
		}
	};
}
#endif
