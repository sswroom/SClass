#ifndef _SM_MATH_UNIT_RATIO
#define _SM_MATH_UNIT_RATIO
namespace Math
{
	namespace Unit
	{
		class Ratio
		{
		public:
			typedef enum
			{
				RU_RATIO,
				RU_PERCENT,

				RU_FIRST = RU_RATIO,
				RU_LAST = RU_PERCENT
			} RatioUnit;
		public:
			static Double GetUnitRatio(RatioUnit unit);
			static const UTF8Char *GetUnitShortName(RatioUnit unit);
			static const UTF8Char *GetUnitName(RatioUnit unit);
			static Double Convert(RatioUnit fromUnit, RatioUnit toUnit, Double fromValue);
		};
	}
}
#endif
