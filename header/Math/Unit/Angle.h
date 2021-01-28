#ifndef _SM_MATH_UNIT_ANGLE
#define _SM_MATH_UNIT_ANGLE
namespace Math
{
	namespace Unit
	{
		class Angle
		{
		public:
			typedef enum
			{
				AU_RADIAN,
				AU_GRADIAN,
				AU_TURN,
				AU_DEGREE,
				AU_ARCMINUTE,
				AU_ARCSECOND,
				AU_MILLIARCSECOND,
				AU_MICROARCSECOND
			} AngleUnit;
		public:
			static Double GetUnitRatio(AngleUnit unit);
			static const UTF8Char *GetUnitShortName(AngleUnit unit);
			static const UTF8Char *GetUnitName(AngleUnit unit);
			static Double Convert(AngleUnit fromUnit, AngleUnit toUnit, Double fromValue);
		};
	}
}
#endif
