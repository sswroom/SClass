#ifndef _SM_MATH_UNIT_ACCELERATION
#define _SM_MATH_UNIT_ACCELERATION
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Acceleration
		{
		public:
			enum class AccelerationUnit
			{
				MeterPerSecondSquared,
				StandardGravity
			};
		public:
			static Double GetUnitRatio(AccelerationUnit unit);
			static Text::CString GetUnitShortName(AccelerationUnit unit);
			static Text::CString GetUnitName(AccelerationUnit unit);
			static Double GetConvertRatio(AccelerationUnit fromUnit, AccelerationUnit toUnit);
			static Double Convert(AccelerationUnit fromUnit, AccelerationUnit toUnit, Double fromValue);
		};
	}
}
#endif
