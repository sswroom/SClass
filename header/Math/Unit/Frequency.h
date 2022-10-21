#ifndef _SM_MATH_UNIT_FREQUENCY
#define _SM_MATH_UNIT_FREQUENCY
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Frequency
		{
		public:
			typedef enum
			{
				FU_HERTZ,
				FU_KILOHERTZ,

				FU_FIRST = FU_HERTZ,
				FU_LAST = FU_KILOHERTZ
			} FrequencyUnit;
		public:
			static Double GetUnitRatio(FrequencyUnit unit);
			static Text::CString GetUnitShortName(FrequencyUnit unit);
			static Text::CString GetUnitName(FrequencyUnit unit);
			static Double GetConvertRatio(FrequencyUnit fromUnit, FrequencyUnit toUnit);
			static Double Convert(FrequencyUnit fromUnit, FrequencyUnit toUnit, Double fromValue);
		};
	}
}
#endif
