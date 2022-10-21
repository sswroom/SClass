#ifndef _SM_MATH_UNIT_REACTIVEPOWER
#define _SM_MATH_UNIT_REACTIVEPOWER
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class ReactivePower
		{
		public:
			typedef enum
			{
				RPU_VAR,

				RPU_FIRST = RPU_VAR,
				RPU_LAST = RPU_VAR
			} ReactivePowerUnit;
		public:
			static Double GetUnitRatio(ReactivePowerUnit unit);
			static Text::CString GetUnitShortName(ReactivePowerUnit unit);
			static Text::CString GetUnitName(ReactivePowerUnit unit);
			static Double GetConvertRatio(ReactivePowerUnit fromUnit, ReactivePowerUnit toUnit);
			static Double Convert(ReactivePowerUnit fromUnit, ReactivePowerUnit toUnit, Double fromValue);
		};
	}
}
#endif
