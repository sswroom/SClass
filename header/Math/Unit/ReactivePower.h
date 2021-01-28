#ifndef _SM_MATH_UNIT_REACTIVEPOWER
#define _SM_MATH_UNIT_REACTIVEPOWER
namespace Math
{
	namespace Unit
	{
		class ReactivePower
		{
		public:
			typedef enum
			{
				RPU_VAR,

				RPU_FIRST = RPU_VAR,
				RPU_LAST = RPU_VAR
			} ReactivePowerUnit;
		public:
			static Double GetUnitRatio(ReactivePowerUnit unit);
			static const UTF8Char *GetUnitShortName(ReactivePowerUnit unit);
			static const UTF8Char *GetUnitName(ReactivePowerUnit unit);
			static Double Convert(ReactivePowerUnit fromUnit, ReactivePowerUnit toUnit, Double fromValue);
		};
	}
}
#endif
