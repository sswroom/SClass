#ifndef _SM_DATA_DURATION
#define _SM_DATA_DURATION
#define DURATION_INFINITY -12345
#include "Text/CString.h"

namespace Data
{
	class Duration
	{
	private:
		Int64 seconds;
		UInt32 ns;

		Duration(Int64 seconds, UInt32 ns)
		{
			this->seconds = seconds;
			this->ns = ns;
		}

	public:
		Duration() = default;
		~Duration() = default;

		Duration(Int64 ticks)
		{
			if (ticks < 0)
			{
				Int32 ns = (Int32)ticks % 1000;
				this->seconds = ticks / 1000;
				if (ns == 0)
				{
					this->ns = 0;
				}
				else
				{
					this->ns = (UInt32)(-ns) * 1000000;
					this->seconds--;
				}
			}
			else
			{
				this->ns = (UInt32)(ticks % 1000) * 1000000;
				this->seconds = ticks / 1000;
			}
		};

		Int64 GetSeconds() const
		{
			return this->seconds;
		}

		UInt32 GetNS() const
		{
			return this->ns;
		}

		Int64 GetTotalMS() const
		{
			return this->seconds * 1000 + this->ns / 1000000;
		}

		Double GetTotalSec() const
		{
			return Int64_Double(this->seconds) + (this->ns * 0.000000001);
		}

		Double GetTotalDays() const
		{
			return GetTotalSec() / 86400.0;
		}

		Bool NotZero() const
		{
			return this->seconds != 0 || this->ns != 0;
		}

		Bool IsZero() const
		{
			return this->seconds == 0 && this->ns == 0;
		}
		
		Bool IsPositiveNonZero() const
		{
			return this->seconds > 0 || (this->seconds == 0 && this->ns > 0);
		}

		Bool IsNegative() const
		{
			return this->seconds < 0;
		}

		Data::Duration AddMS(Int64 ms) const
		{
			Int64 secs = ms / 1000;
			UInt32 ns;
			if (ms < 0)
			{
				ns = (UInt32)((ms % 1000) + 1000) * 1000000 + this->ns;
				secs -= 1;
			}
			else
			{
				ns = (UInt32)(ms % 1000) * 1000000 + this->ns;
			}
			if (ns >= 1000000000)
			{
				return Data::Duration(this->seconds + secs + 1, ns - 1000000000);
			}
			else
			{
				return Data::Duration(this->seconds + secs, ns);
			}
		}

		Data::Duration AddSecDbl(Double sec) const
		{
			Int64 isec = (Int64)sec;
			Int64 ns = this->ns + (Int64)((sec - (Double)isec) * 1000000000);
			if (ns < 0)
			{
				ns += 1000000000;
				isec -= 1;
			}
			else if (ns >= 1000000000)
			{
				ns -= 1000000000;
				isec += 1;
			}
			return Data::Duration(isec + this->seconds, (UInt32)ns);
		}

		Bool operator>=(Data::Duration dur) const
		{
			if (this->seconds < dur.seconds)
				return false;
			else if (this->seconds > dur.seconds)
				return true;
			else
				return this->ns >= dur.ns;
		}

		Bool operator>(Data::Duration dur) const
		{
			if (this->seconds < dur.seconds)
				return false;
			else if (this->seconds > dur.seconds)
				return true;
			else
				return this->ns > dur.ns;
		}

		Bool operator<=(Data::Duration dur) const
		{
			if (this->seconds > dur.seconds)
				return false;
			else if (this->seconds < dur.seconds)
				return true;
			else
				return this->ns <= dur.ns;
		}

		Bool operator<(Data::Duration dur) const
		{
			if (this->seconds > dur.seconds)
				return false;
			else if (this->seconds < dur.seconds)
				return true;
			else
				return this->ns < dur.ns;
		}

		Bool operator==(Data::Duration dur) const
		{
			return this->seconds == dur.seconds && this->ns == dur.ns;
		}

		Bool operator!=(Data::Duration dur) const
		{
			return this->seconds != dur.seconds || this->ns != dur.ns;
		}

		Bool operator>=(Int64 ticks) const = delete;
		Bool operator>(Int64 ticks) const = delete;
		Bool operator<=(Int64 ticks) const = delete;
		Bool operator<(Int64 ticks) const = delete;
		Bool operator==(Int64 ticks) const = delete;
		Bool operator!=(Int64 ticks) const = delete;

		Data::Duration operator+(Data::Duration dur) const
		{
			UInt32 ns = this->ns + dur.ns;
			if (ns >= 1000000000)
				return Data::Duration(this->seconds + dur.GetSeconds() + 1, ns - 1000000000);
			else
				return Data::Duration(this->seconds + dur.GetSeconds(), ns);
		}

		const Data::Duration &operator+=(Data::Duration dur)
		{
			this->ns = this->ns + dur.ns;
			if (this->ns >= 1000000000)
			{
				this->ns -= 1000000000;
				this->seconds += dur.seconds + 1;
			}
			else
			{
				this->seconds += dur.seconds;
			}
			return NNTHIS;
		}

		Data::Duration operator-(Data::Duration dur) const
		{
			Int64 secs = this->seconds - dur.seconds;
			UInt32 ns1 = this->ns;
			UInt32 ns2 = dur.ns;
			if (ns1 >= ns2)
				return Data::Duration(secs, ns1 - ns2);
			else
				return Data::Duration(secs - 1, 1000000000 + ns1 - ns2);
		}

		const Data::Duration &operator-=(Data::Duration dur)
		{
			this->seconds = this->seconds + dur.seconds;
			if (this->ns >= dur.ns)
			{
				this->ns -= dur.ns;
			}
			else
			{
				this->seconds -= 1;
				this->ns = 1000000000 + this->ns - dur.ns;
			}
			return NNTHIS;
		}


		Data::Duration operator/(UInt32 v) const
		{
			Int64 v2 = this->seconds % (Int64)v;
			Int64 seconds = this->seconds / (Int64)v;
			if (v2 < 0)
			{
				seconds--;
				v2 += v;
			}
			return Data::Duration(seconds, (UInt32)((UInt64)v2 * 1000000000 + this->ns) / v);
		}

		Bool IsInfinity() const
		{
			return this->seconds == DURATION_INFINITY;
		}

		UInt64 MultiplyU64(UInt64 multiplier) const
		{
			if (this->seconds < 0)
				return 0;
			return ((UInt64)this->seconds * multiplier) + (this->ns * multiplier / 1000000000);
		}

		UInt64 SecsMulDivU32(UInt32 multiplier, UInt32 divider) const
		{
			if (this->seconds < 0)
				return 0;
			UInt64 secs = (UInt64)this->seconds;
			UInt64 ns = this->ns;
			secs = secs * multiplier + (ns * multiplier) / 1000000000;
			return secs / divider;
		}

		static Data::Duration FromUs(Int64 us)
		{
			if (us < 0)
			{
				Int32 ns = (Int32)us % 1000000;
				Int64 seconds = us / 1000000;
				if (ns == 0)
					return Data::Duration(seconds, 0);
				else
					return Data::Duration(seconds - 1, (UInt32)(-ns) * 1000);
			}
			else
			{
				return Data::Duration(us / 1000000, (UInt32)(us % 1000000) * 1000);
			}
		}

		static Data::Duration Infinity()
		{
			return Data::Duration(DURATION_INFINITY, 0);
		}

		static Data::Duration FromSecNS(Int64 seconds, UInt32 ns)
		{
			return Data::Duration(seconds, ns);
		}
		
		static Data::Duration FromRatioU64(UInt64 val, UInt64 divider)
		{
			Int64 secs = (Int64)(val / divider);
			UInt64 bytes = val % divider;
			return Data::Duration(secs, (UInt32)(bytes * 1000000000 / divider));
		}

		static Data::Duration FromStr(Text::CStringNN s)
		{
			UTF8Char sbuff[64];
			UnsafeArray<UTF8Char> sptr;
			if (s.leng >= 64 || s.leng == 0)
			{
				return 0;
			}
			Double timeMul = 0.001;
			sptr = s.ConcatTo(sbuff);
			if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("ms")))
			{
				sptr -= 2;
				sptr[0] = 0;
			}
			else if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("us")))
			{
				timeMul = 0.000001;
				sptr -= 2;
				sptr[0] = 0;
			}
			else if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("ns")))
			{
				timeMul = 0.000000001;
				sptr -= 2;
				sptr[0] = 0;
			}
			else if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("min")))
			{
				timeMul = 60;
				sptr -= 3;
				sptr[0] = 0;
			}
			else if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("h")))
			{
				timeMul = 3600;
				sptr -= 1;
				sptr[0] = 0;
			}
			else if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("s")))
			{
				timeMul = 1;
				sptr -= 1;
				sptr[0] = 0;
			}
			Double val;
			if (!Text::StrToDouble(sbuff, val))
			{
				return 0;
			}
			val *= timeMul;
			Int64 iVal;
			UInt32 ns;
			iVal = (Int64)Math_Fix(val);
			if (val < 0)
			{
				iVal -= 1;
				ns = (UInt32)Math_Round((val - (Double)iVal) * 1000000000);
				if (ns >= 1000000000)
				{
					ns -= 1000000000;
					iVal += 1;
				}
			}
			else
			{
				ns = (UInt32)Math_Round((val - (Double)iVal) * 1000000000);
			}
			return Data::Duration(iVal, ns);
		}
	};
}
#endif
