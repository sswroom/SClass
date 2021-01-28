#ifndef _SM_MATH_UNIT_ELECTRICCURRENT
#define _SM_MATH_UNIT_ELECTRICCURRENT
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
			static const UTF8Char *GetUnitShortName(ElectricCurrentUnit unit);
			static const UTF8Char *GetUnitName(ElectricCurrentUnit unit);
			static Double Convert(ElectricCurrentUnit fromUnit, ElectricCurrentUnit toUnit, Double fromValue);
		};
	}
}
#endif
