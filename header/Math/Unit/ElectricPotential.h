#ifndef _SM_MATH_UNIT_ELECTRICPOTENTIAL
#define _SM_MATH_UNIT_ELECTRICPOTENTIAL
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class ElectricPotential
		{
		public:
			typedef enum
			{
				EPU_VOLT,
				EPU_MILLIVOLT,

				EPU_FIRST = EPU_VOLT,
				EPU_LAST = EPU_MILLIVOLT
			} ElectricPotentialUnit;
		public:
			static Double GetUnitRatio(ElectricPotentialUnit unit);
			static Text::CStringNN GetUnitShortName(ElectricPotentialUnit unit);
			static Text::CStringNN GetUnitName(ElectricPotentialUnit unit);
			static Double GetConvertRatio(ElectricPotentialUnit fromUnit, ElectricPotentialUnit toUnit);
			static Double Convert(ElectricPotentialUnit fromUnit, ElectricPotentialUnit toUnit, Double fromValue);
		};
	}
}
#endif
