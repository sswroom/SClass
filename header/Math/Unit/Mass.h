#ifndef _SM_MATH_UNIT_MASS
#define _SM_MATH_UNIT_MASS
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
			static const UTF8Char *GetUnitShortName(MassUnit unit);
			static const UTF8Char *GetUnitName(MassUnit unit);
			static Double Convert(MassUnit fromUnit, MassUnit toUnit, Double fromValue);
		};
	}
}
#endif
