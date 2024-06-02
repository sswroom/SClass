#ifndef _SM_MATH_UNIT_REACTIVEENERGY
#define _SM_MATH_UNIT_REACTIVEENERGY
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class ReactiveEnergy
		{
		public:
			typedef enum
			{
				REU_KVARH,

				REU_FIRST = REU_KVARH,
				REU_LAST = REU_KVARH
			} ReactiveEnergyUnit;
		public:
			static Double GetUnitRatio(ReactiveEnergyUnit unit);
			static Text::CStringNN GetUnitShortName(ReactiveEnergyUnit unit);
			static Text::CStringNN GetUnitName(ReactiveEnergyUnit unit);
			static Double GetConvertRatio(ReactiveEnergyUnit fromUnit, ReactiveEnergyUnit toUnit);
			static Double Convert(ReactiveEnergyUnit fromUnit, ReactiveEnergyUnit toUnit, Double fromValue);
		};
	}
}
#endif
