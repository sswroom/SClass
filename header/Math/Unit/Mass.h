#ifndef _SM_MATH_UNIT_MASS
#define _SM_MATH_UNIT_MASS
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Mass
		{
		public:
			typedef enum
			{
				MU_KILOGRAM,
				MU_GRAM,
				MU_TONNE,
				MU_POUND,
				MU_OZ
			} MassUnit;
		public:
			static Double GetUnitRatio(MassUnit unit);
			static Text::CString GetUnitShortName(MassUnit unit);
			static Text::CString GetUnitName(MassUnit unit);
			static Double GetConvertRatio(MassUnit fromUnit, MassUnit toUnit);
			static Double Convert(MassUnit fromUnit, MassUnit toUnit, Double fromValue);
		};
	}
}
#endif
