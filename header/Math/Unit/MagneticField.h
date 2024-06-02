#ifndef _SM_MATH_UNIT_MAGNETICFIELD
#define _SM_MATH_UNIT_MAGNETICFIELD
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class MagneticField
		{
		public:
			typedef enum
			{
				MFU_TESLA,
				MFU_GAUSS,
				MFU_MICROTESLA,
				MFU_MILLITESLA
			} MagneticFieldUnit;
		public:
			static Double GetUnitRatio(MagneticFieldUnit unit);
			static Text::CStringNN GetUnitShortName(MagneticFieldUnit unit);
			static Text::CStringNN GetUnitName(MagneticFieldUnit unit);
			static Double GetConvertRatio(MagneticFieldUnit fromUnit, MagneticFieldUnit toUnit);
			static Double Convert(MagneticFieldUnit fromUnit, MagneticFieldUnit toUnit, Double fromValue);
		};
	}
}
#endif
