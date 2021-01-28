#ifndef _SM_MATH_UNIT_POWER
#define _SM_MATH_UNIT_POWER
namespace Math
{
	namespace Unit
	{
		class Power
		{
		public:
			typedef enum
			{
				PU_WATT,
				PU_MILLIVOLT,
				PU_KILOVOLT,

				PU_FIRST = PU_WATT,
				PU_LAST = PU_KILOVOLT
			} PowerUnit;
		public:
			static Double GetUnitRatio(PowerUnit unit);
			static const UTF8Char *GetUnitShortName(PowerUnit unit);
			static const UTF8Char *GetUnitName(PowerUnit unit);
			static Double Convert(PowerUnit fromUnit, PowerUnit toUnit, Double fromValue);
		};
	}
}
#endif
