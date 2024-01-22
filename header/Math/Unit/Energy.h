#ifndef _SM_MATH_UNIT_ENERGY
#define _SM_MATH_UNIT_ENERGY
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Energy
		{
		public:
			typedef enum
			{
				EU_JOULE,
				EU_WATTHOUR,
				EU_KILOWATTHOUR,
				EU_CALORIE,
				EU_KILOCALORIE,

				EU_FIRST = EU_JOULE,
				EU_LAST = EU_KILOCALORIE
			} EnergyUnit;
		public:
			static Double GetUnitRatio(EnergyUnit unit);
			static Text::CString GetUnitShortName(EnergyUnit unit);
			static Text::CString GetUnitName(EnergyUnit unit);
			static Double GetConvertRatio(EnergyUnit fromUnit, EnergyUnit toUnit);
			static Double Convert(EnergyUnit fromUnit, EnergyUnit toUnit, Double fromValue);
		};
	}
}
#endif
