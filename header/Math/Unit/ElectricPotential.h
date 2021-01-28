#ifndef _SM_MATH_UNIT_ELECTRICPOTENTIAL
#define _SM_MATH_UNIT_ELECTRICPOTENTIAL
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
			static const UTF8Char *GetUnitShortName(ElectricPotentialUnit unit);
			static const UTF8Char *GetUnitName(ElectricPotentialUnit unit);
			static Double Convert(ElectricPotentialUnit fromUnit, ElectricPotentialUnit toUnit, Double fromValue);
		};
	}
}
#endif
