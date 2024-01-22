#ifndef _SM_MATH_UNIT_POWER
#define _SM_MATH_UNIT_POWER
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Power
		{
		public:
			typedef enum
			{
				PU_WATT,
				PU_MILLIWATT,
				PU_KILOWATT,

				PU_FIRST = PU_WATT,
				PU_LAST = PU_KILOWATT
			} PowerUnit;
		public:
			static Double GetUnitRatio(PowerUnit unit);
			static Text::CString GetUnitShortName(PowerUnit unit);
			static Text::CString GetUnitName(PowerUnit unit);
			static Double GetConvertRatio(PowerUnit fromUnit, PowerUnit toUnit);
			static Double Convert(PowerUnit fromUnit, PowerUnit toUnit, Double fromValue);
		};
	}
}
#endif
