#ifndef _SM_MATH_UNIT_DISTANCE
#define _SM_MATH_UNIT_DISTANCE
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Distance
		{
		public:
			typedef enum
			{
				DU_METER,
				DU_CENTIMETER,
				DU_MILLIMETER,
				DU_MICROMETER,
				DU_NANOMETER,
				DU_PICOMETER,
				DU_KILOMETER,
				DU_INCH,
				DU_FOOT,
				DU_YARD,
				DU_MILE,
				DU_NAUTICAL_MILE,
				DU_AU,
				DU_LIGHTSECOND,
				DU_LIGHTMINUTE,
				DU_LIGHTHOUR,
				DU_LIGHTDAY,
				DU_LIGHTWEEK,
				DU_LIGHTYEAR,
				DU_EMU,
				DU_POINT,
				DU_PIXEL,
				DU_TWIP,
				DU_CLARKE_FOOT,

				DU_FIRST = DU_METER,
				DU_LAST = DU_CLARKE_FOOT
			} DistanceUnit;
		public:
			static Double GetUnitRatio(DistanceUnit unit);
			static Text::CStringNN GetUnitShortName(DistanceUnit unit);
			static Text::CStringNN GetUnitName(DistanceUnit unit);
			static Double GetConvertRatio(DistanceUnit fromUnit, DistanceUnit toUnit);
			static Double Convert(DistanceUnit fromUnit, DistanceUnit toUnit, Double fromValue);
		};
	}
}
#endif
