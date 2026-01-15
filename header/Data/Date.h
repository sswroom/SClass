#ifndef _SM_DATA_DATE
#define _SM_DATA_DATE
#include "Data/DateTimeUtil.h"

#define DATE_NULL -1234567

namespace Data
{
	class Date
	{
	private:
		Int64 dateVal;
	public:
		Date() = default;

		Date(std::nullptr_t)
		{
			this->dateVal = DATE_NULL;
		}

		Date(Int64 totalDays)
		{
			this->dateVal = totalDays;
		}

		Date(const DateTimeUtil::DateValue dateVal)
		{
			this->dateVal = DateTimeUtil::DateValue2TotalDays(dateVal);
		}

		Date(Int32 year, UInt8 month, UInt8 day)
		{
			this->dateVal = DateTimeUtil::Date2TotalDays(year, month, day);
		}

		Date(Text::CStringNN dateStr)
		{
			Data::DateTimeUtil::TimeValue tval;
			UInt32 nanosec;
			Int8 tzQhr;
			if (DateTimeUtil::String2TimeValue(dateStr, tval, 0, tzQhr, nanosec))
			{
				this->dateVal = DateTimeUtil::DateValue2TotalDays(tval);
			}
			else
			{
				this->dateVal = DATE_NULL;
			}
		}

		~Date()
		{
		}

		void SetValue(Int32 year, UInt8 month, UInt8 day)
		{
			this->dateVal = DateTimeUtil::Date2TotalDays(year, month, day);
		}

		DateTimeUtil::DateValue GetDateValue() const
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			return d;
		}

		Int64 GetTotalDays() const
		{
			return this->dateVal;
		}

		void SetYear(Int32 year)
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			this->dateVal = DateTimeUtil::Date2TotalDays(year, d.month, d.day);
		}

		void SetMonth(IntOS month)
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			this->dateVal = DateTimeUtil::Date2TotalDays(d.year, (Int32)month, d.day);
		}

		void SetDay(IntOS day)
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			this->dateVal = DateTimeUtil::Date2TotalDays(d.year, d.month, (Int32)day);
		}

		Data::Date AddDay(IntOS day) const
		{
			return Data::Date(this->dateVal + day);
		}

		Bool IsYearLeap() const
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			return DateTimeUtil::IsYearLeap(d.year);
		}

		Int64 ToTicks() const
		{
			return this->dateVal * 86400000LL;
		}

		Int64 ToTicks(Int8 tzQhr) const
		{
			return this->dateVal * 86400000LL - tzQhr * 900000LL;
		}

		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff) const
		{
			return ToString(buff, "yyyy-MM-dd");
		}

		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff, const Char *pattern) const
		{
			DateTimeUtil::TimeValue t;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, t);
			t.hour = 0;
			t.minute = 0;
			t.second = 0;
			return DateTimeUtil::ToString(buff, t, 0, 0, (const UTF8Char*)pattern);
		}

		Bool operator==(const Date &dt) const
		{
			return this->dateVal == dt.dateVal;
		}

		Bool operator>(const Date &dt) const
		{
			return (this->dateVal > dt.dateVal);
		}

		Bool operator<(const Date &dt) const
		{
			return (this->dateVal < dt.dateVal);
		}
		
		Bool operator>=(const Date &dt) const
		{
			return (this->dateVal >= dt.dateVal);
		}

		Bool operator<=(const Date &dt) const
		{
			return (this->dateVal <= dt.dateVal);
		}
		
		IntOS CompareTo(NN<const Data::Date> obj) const
		{
			if (this->dateVal > obj->dateVal)
				return 1;
			else if (this->dateVal < obj->dateVal)
				return -1;
			else
				return 0;
		}

		Bool IsNull() const
		{
			return this->dateVal == DATE_NULL;
		}

		DateTimeUtil::Weekday GetWeekday() const
		{
			return DateTimeUtil::Ticks2Weekday(this->dateVal * 86400000, 0);
		}

		Int32 ToYMD() const
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			if (d.year <= 0)
			{
				return (d.year - 1) * 10000 - (Int32)d.month * 100 - (Int32)d.day;
			}
			else
			{
				return d.year * 10000 + (Int32)d.month * 100 + (Int32)d.day;
			}
		}

		static Date Today()
		{
			return Data::Date((Data::DateTimeUtil::GetCurrTimeMillis() + Data::DateTimeUtil::GetLocalTzQhr() * 900000) / 86400000);
		}
	};
}
#endif
