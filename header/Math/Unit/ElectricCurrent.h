#ifndef _SM_MATH_UNIT_ELECTRICCURRENT
#define _SM_MATH_UNIT_ELECTRICCURRENT
#include "Text/CString.h"
namespace Math
{
	namespace Unit
	{
		class ElectricCurrent
		{
		public:
			typedef enum
			{
				ECU_AMPERE,
				ECU_MILLIAMPERE,
				ECU_MICROAMPERE,

				ECU_FIRST = ECU_AMPERE,
				ECU_LAST = ECU_MICROAMPERE
			} ElectricCurrentUnit;
		public:
			static Double GetUnitRatio(ElectricCurrentUnit unit);
			static Text::CString GetUnitShortName(ElectricCurrentUnit unit);
			static Text::CString GetUnitName(ElectricCurrentUnit unit);
			static Double GetConvertRatio(ElectricCurrentUnit fromUnit, ElectricCurrentUnit toUnit);
			static Double Convert(ElectricCurrentUnit fromUnit, ElectricCurrentUnit toUnit, Double fromValue);
		};
	}
}
#endif
