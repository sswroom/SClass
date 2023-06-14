#ifndef _SM_DATA_DURATION
#define _SM_DATA_DURATION

namespace Data
{
	class Duration
	{
	private:
		Int64 seconds;
		UInt32 ns;

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

		Duration(Int64 seconds, UInt32 ns)
		{
			this->seconds = seconds;
			this->ns = ns;
		}

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

		Bool NotZero() const
		{
			return this->seconds != 0 || this->ns != 0;
		}

		Bool IsZero() const
		{
			return this->seconds == 0 && this->ns == 0;
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

		static Data::Duration FromUs(Int64 us)
		{
			if (us < 0)
			{
				Int32 ns = (Int32)us % 1000000;
				Int64 seconds = us / 1000000;
				if (ns != 0)
					return Data::Duration(seconds, 0);
				else
					return Data::Duration(seconds - 1, (UInt32)(-ns) * 1000);
			}
			else
			{
				return Data::Duration(us / 1000000, (UInt32)(us % 1000000) * 1000);
			}
		}
	};
}
#endif
