#ifndef _SM_MATH_UNIT_RATIO
#define _SM_MATH_UNIT_RATIO
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Ratio
		{
		public:
			typedef enum
			{
				RU_RATIO,
				RU_PERCENT,

				RU_FIRST = RU_RATIO,
				RU_LAST = RU_PERCENT
			} RatioUnit;
		public:
			static Double GetUnitRatio(RatioUnit unit);
			static Text::CString GetUnitShortName(RatioUnit unit);
			static Text::CString GetUnitName(RatioUnit unit);
			static Double Convert(RatioUnit fromUnit, RatioUnit toUnit, Double fromValue);
		};
	}
}
#endif
