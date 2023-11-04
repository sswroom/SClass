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

		void SetMonth(OSInt month)
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			this->dateVal = DateTimeUtil::Date2TotalDays(d.year, (Int32)month, d.day);
		}

		void SetDay(OSInt day)
		{
			DateTimeUtil::DateValue d;
			DateTimeUtil::TotalDays2DateValue(this->dateVal, d);
			this->dateVal = DateTimeUtil::Date2TotalDays(d.year, d.month, (Int32)day);
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

		UTF8Char *ToString(UTF8Char *buff) const
		{
			return ToString(buff, "yyyy-MM-dd");
		}

		UTF8Char *ToString(UTF8Char *buff, const Char *pattern) const
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
		
		OSInt CompareTo(NotNullPtr<const Data::Date> obj) const
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
	};
}
#endif
