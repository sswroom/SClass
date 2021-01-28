#ifndef _SM_MATH_UNIT_COUNT
#define _SM_MATH_UNIT_COUNT
namespace Math
{
	namespace Unit
	{
		class Count
		{
		public:
			typedef enum
			{
				CU_UNIT,

				CU_FIRST = CU_UNIT,
				CU_LAST = CU_UNIT
			} CountUnit;
		public:
			static Double GetUnitRatio(CountUnit unit);
			static const UTF8Char *GetUnitShortName(CountUnit unit);
			static const UTF8Char *GetUnitName(CountUnit unit);
			static Double Convert(CountUnit fromUnit, CountUnit toUnit, Double fromValue);
		};
	}
}
#endif
