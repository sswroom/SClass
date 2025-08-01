#ifndef _SM_DATA_TIMEINSTANT
#define _SM_DATA_TIMEINSTANT
#include "Data/DateTimeUtil.h"
#include "Data/Duration.h"

namespace Data
{
	class TimeInstant
	{
	public:
		Int64 sec;
		UInt32 nanosec;

	public:
		TimeInstant() = default;

		TimeInstant(Int64 sec, UInt32 nanosec)
		{
			this->sec = sec;
			this->nanosec = nanosec;
		};

		~TimeInstant() = default;

		TimeInstant AddDay(OSInt val) const
		{
			return TimeInstant(this->sec + val * 86400LL, this->nanosec);
		}

		TimeInstant AddHour(OSInt val) const
		{
			return TimeInstant(this->sec + val * 3600LL, this->nanosec);
		}

		TimeInstant AddMinute(OSInt val) const
		{
			return TimeInstant(this->sec + val * 60LL, this->nanosec);
		}

		TimeInstant AddSecond(OSInt val) const
		{
			return TimeInstant(this->sec + val, this->nanosec);
		}

		TimeInstant AddMS(OSInt val) const
		{
			Int64 newSec = this->sec + val / 1000;
			val = (val % 1000) * 1000000 + (Int32)this->nanosec;
			while (val > 1000000000)
			{
				newSec++;
				val -= 1000000000;
			}
			return TimeInstant(newSec, (UInt32)val);
		}

		TimeInstant AddNS(Int64 val) const
		{
			Int64 newSec = this->sec + val / 1000000000;
			val = val % 1000000000 + (Int32)this->nanosec;
			while (val > 1000000000)
			{
				newSec++;
				val -= 1000000000;
			}
			return TimeInstant(newSec, (UInt32)val);
		}

		UInt32 GetMS() const
		{
			return (UInt32)(this->nanosec / 1000000);
		}

		TimeInstant ClearTime() const
		{
			return TimeInstant(this->sec - this->sec % 86400, 0);
		}

		TimeInstant RoundToS() const
		{
			if (this->nanosec >= 500000000)
			{
				return TimeInstant(this->sec + 1, 0);
			}
			else
			{
				return TimeInstant(this->sec, 0);
			}
		}

		Int64 GetMSPassedDate() const
		{
			return (this->sec % 86400LL) * 1000 + this->nanosec / 1000000;
		};

		Int64 DiffMS(const TimeInstant &ts) const
		{
			return (this->sec - ts.sec) * 1000LL + (this->nanosec / 1000000) - (ts.nanosec / 1000000);
		}

		Int64 DiffSec(const TimeInstant &ts) const
		{
			return this->sec - ts.sec;
		}

		Double DiffSecDbl(const TimeInstant &ts) const
		{
			return (Double)(this->sec - ts.sec) + (Int32)(this->nanosec - ts.nanosec) / 1000000000.0;
		}

		Data::Duration Diff(const TimeInstant &ts) const
		{
			Int64 secs = this->sec - ts.sec;
			UInt32 ns1 = this->nanosec;
			UInt32 ns2 = ts.nanosec;
			if (ns1 >= ns2)
				return Data::Duration::FromSecNS(secs, ns1 - ns2);
			else
				return Data::Duration::FromSecNS(secs - 1, 1000000000 + ns1 - ns2);
		}

		Int64 ToTicks() const
		{
			return this->sec * 1000LL + (this->nanosec / 1000000);
		}

		Int64 ToDotNetTicks() const
		{
			return this->sec * 10000000LL + 621355968000000000LL + (this->nanosec / 100);
		}

		Int64 ToUnixTimestamp() const
		{
			return this->sec;
		}

		Int64 ToEpochSec() const
		{
			return this->sec;
		}

		Int64 ToEpochMS() const
		{
			return this->sec * 1000LL + (this->nanosec / 1000000);
		}

		Int64 ToEpochNS() const
		{
			return this->sec * 1000000000LL + this->nanosec;
		}

		void ToFILETIME(void* fileTime) const
		{
			Data::DateTimeUtil::Secs2FILETIME(this->sec, this->nanosec, fileTime);
		}

		static TimeInstant FromDotNetTicks(Int64 ticks)
		{
			ticks -= 621355968000000000LL;
			Int32 ns = (Int32)(ticks % 10000000);
			if (ns < 0)
			{
				return TimeInstant(ticks / 10000000LL - 1, (UInt32)(ns + 10000000) * 100);
			}
			else
			{
				return TimeInstant(ticks / 10000000LL, (UInt32)ns * 100);
			}
		}

		static TimeInstant FromUnixTimestamp(Int64 ts)
		{
			return TimeInstant(ts, 0);
		}

		static TimeInstant FromFILETIME(void *fileTime)
		{
			Int64 t = ReadInt64((const UInt8*)fileTime) - 116444736000000000LL;
			Int32 ns = (Int32)(t % 10000000);
			if (ns < 0)
			{
				return TimeInstant(t / 10000000LL - 1, (UInt32)(ns + 10000000) * 100);
			}
			else
			{
				return TimeInstant(t / 10000000LL, (UInt32)ns * 100);
			}
		}

		TimeInstant operator+(Duration dur) const
		{
			Int64 sec = this->sec + dur.GetSeconds();
			UInt32 ns = this->nanosec + dur.GetNS();
			if (ns >= 1000000000)
				return TimeInstant(sec + 1, ns - 1000000000);
			else
				return TimeInstant(sec, ns);
		}

		TimeInstant operator-(Duration dur) const
		{
			Int64 sec = this->sec - dur.GetSeconds();
			UInt32 ns1 = this->nanosec;
			UInt32 ns2 = dur.GetNS();
			if (ns1 >= ns2)
				return TimeInstant(sec, ns1 - ns2);
			else
				return TimeInstant(sec - 1, 1000000000 + ns1 - ns2);
		}

		Bool operator==(TimeInstant dt) const
		{
			return this->sec == dt.sec && this->nanosec == dt.nanosec;
		}

		Bool operator!=(TimeInstant dt) const
		{
			return this->sec != dt.sec || this->nanosec != dt.nanosec;
		}

		Bool operator>=(TimeInstant dt) const
		{
			if (this->sec < dt.sec)
				return false;
			else if (this->sec > dt.sec)
				return true;
			else if (this->nanosec < dt.nanosec)
				return false;
			else
				return true;
		}

		Bool operator<=(TimeInstant dt) const
		{
			if (this->sec > dt.sec)
				return false;
			else if (this->sec < dt.sec)
				return true;
			else if (this->nanosec > dt.nanosec)
				return false;
			else
				return true;
		}

		Bool operator>(TimeInstant dt) const
		{
			if (this->sec < dt.sec)
				return false;
			else if (this->sec > dt.sec)
				return true;
			else if (this->nanosec <= dt.nanosec)
				return false;
			else
				return true;
		}

		Bool operator<(TimeInstant dt) const
		{
			if (this->sec > dt.sec)
				return false;
			else if (this->sec < dt.sec)
				return true;
			else if (this->nanosec >= dt.nanosec)
				return false;
			else
				return true;
		}

		OSInt CompareTo(const TimeInstant& ts) const
		{
			if (this->sec > ts.sec)
				return 1;
			else if (this->sec < ts.sec)
				return -1;
			else if (this->nanosec > ts.nanosec)
				return 1;
			else if (this->nanosec < ts.nanosec)
				return -1;
			else
				return 0;
		}

		Bool SameDate(Data::TimeInstant ts) const
		{
			return (this->sec / 86400) == (ts.sec / 86400);
		}

		Bool IsZero() const
		{
			return this->sec == 0 && this->nanosec == 0;
		}

		static TimeInstant Now()
		{
			UInt32 nanosec;
			Int64 secs = Data::DateTimeUtil::GetCurrTimeSecHighP(nanosec);
			return TimeInstant(secs, nanosec);
		}

		static TimeInstant FromVariTime(Double variTime)
		{
			Int32 days = (Int32)variTime;
			Double ds = (variTime - days);
			OSInt s = (OSInt)(ds * 86400);
			return Data::TimeInstant(((Int64)days - 25569) * 86400000LL + (OSInt)(ds * 86400000), (UInt32)((ds * 86400 - (Double)s) * 1000000000));
		}

		static TimeInstant FromTicks(Int64 ticks)
		{
			Int32 ms = (Int32)(ticks % 1000);
			if (ms < 0)
			{
				return Data::TimeInstant(ticks / 1000LL - 1, (UInt32)(ms + 1000) * 1000000);
			}
			else
			{
				return Data::TimeInstant(ticks / 1000LL, (UInt32)ms * 1000000);
			}
		}
	};
}
#endif
