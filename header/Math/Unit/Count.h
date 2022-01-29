#ifndef _SM_MATH_UNIT_COUNT
#define _SM_MATH_UNIT_COUNT
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Count
		{
		public:
			typedef enum
			{
				CU_UNIT,
				CU_KUNIT,
				CU_KIUNIT,
				CU_MUNIT,
				CU_MIUNIT,
				CU_GUNIT,
				CU_GIUNIT,
				CU_TUNIT,
				CU_TIUNIT,

				CU_FIRST = CU_UNIT,
				CU_LAST = CU_TIUNIT
			} CountUnit;
		public:
			static Double GetUnitRatio(CountUnit unit);
			static Text::CString GetUnitShortName(CountUnit unit);
			static Text::CString GetUnitName(CountUnit unit);
			static Double Convert(CountUnit fromUnit, CountUnit toUnit, Double fromValue);
			static UTF8Char *WellFormat(UTF8Char *sbuff, Double val);
			static UTF8Char *WellFormatBin(UTF8Char *sbuff, Double val);
		};
	}
}
#endif
