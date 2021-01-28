#ifndef _SM_MATH_UNIT_FREQUENCY
#define _SM_MATH_UNIT_FREQUENCY
namespace Math
{
	namespace Unit
	{
		class Frequency
		{
		public:
			typedef enum
			{
				FU_HERTZ,
				FU_KILOHERTZ,

				FU_FIRST = FU_HERTZ,
				FU_LAST = FU_KILOHERTZ
			} FrequencyUnit;
		public:
			static Double GetUnitRatio(FrequencyUnit unit);
			static const UTF8Char *GetUnitShortName(FrequencyUnit unit);
			static const UTF8Char *GetUnitName(FrequencyUnit unit);
			static Double Convert(FrequencyUnit fromUnit, FrequencyUnit toUnit, Double fromValue);
		};
	}
}
#endif
