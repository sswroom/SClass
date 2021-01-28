#ifndef _SM_MATH_UNIT_ACCELERATION
#define _SM_MATH_UNIT_ACCELERATION
namespace Math
{
	namespace Unit
	{
		class Acceleration
		{
		public:
			typedef enum
			{
				AU_METER_PER_SECOND_SQUARED,
				AU_STANDARD_GRAVITY
			} AccelerationUnit;
		public:
			static Double GetUnitRatio(AccelerationUnit unit);
			static const UTF8Char *GetUnitShortName(AccelerationUnit unit);
			static const UTF8Char *GetUnitName(AccelerationUnit unit);
			static Double Convert(AccelerationUnit fromUnit, AccelerationUnit toUnit, Double fromValue);
		};
	}
}
#endif
