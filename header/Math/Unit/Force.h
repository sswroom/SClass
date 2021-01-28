#ifndef _SM_MATH_UNIT_FORCE
#define _SM_MATH_UNIT_FORCE
namespace Math
{
	namespace Unit
	{
		class Force
		{
		public:
			typedef enum
			{
				FU_NEWTON
			} ForceUnit;
		public:
			static Double GetUnitRatio(ForceUnit unit);
			static const UTF8Char *GetUnitShortName(ForceUnit unit);
			static const UTF8Char *GetUnitName(ForceUnit unit);
			static Double Convert(ForceUnit fromUnit, ForceUnit toUnit, Double fromValue);
		};
	}
}
#endif
