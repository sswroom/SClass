#ifndef _SM_MATH_UNIT_REACTIVEENERGY
#define _SM_MATH_UNIT_REACTIVEENERGY
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
			static const UTF8Char *GetUnitShortName(ReactiveEnergyUnit unit);
			static const UTF8Char *GetUnitName(ReactiveEnergyUnit unit);
			static Double Convert(ReactiveEnergyUnit fromUnit, ReactiveEnergyUnit toUnit, Double fromValue);
		};
	}
}
#endif
