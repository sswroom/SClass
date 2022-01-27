#ifndef _SM_MATH_UNIT_APPARENTPOWER
#define _SM_MATH_UNIT_APPARENTPOWER
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class ApparentPower
		{
		public:
			typedef enum
			{
				APU_VOLTAMPERE,

				APU_FIRST = APU_VOLTAMPERE,
				APU_LAST = APU_VOLTAMPERE
			} ApparentPowerUnit;
		public:
			static Double GetUnitRatio(ApparentPowerUnit unit);
			static Text::CString GetUnitShortName(ApparentPowerUnit unit);
			static Text::CString GetUnitName(ApparentPowerUnit unit);
			static Double Convert(ApparentPowerUnit fromUnit, ApparentPowerUnit toUnit, Double fromValue);
		};
	}
}
#endif
