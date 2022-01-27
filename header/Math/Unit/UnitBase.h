#ifndef _SM_MATH_UNIT_UNITBASE
#define _SM_MATH_UNIT_UNITBASE
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class UnitBase
		{
		public:
			typedef enum
			{
				VT_MAPPING,
				VT_ELECTRIC_POTENTIAL,
				VT_ELECTRIC_CURRENT,
				VT_POWER,
				VT_ANGLE,
				VT_FREQUENCY,
				VT_RATIO,
				VT_APPARENT_POWER,
				VT_REACTIVE_POWER,
				VT_ENERGY,
				VT_REACTIVE_ENERGY,
				VT_COUNT,
				VT_TEMPERATURE
			} ValueType;
		public:
			static Text::CString GetUnitShortName(ValueType vt, Int32 unit);
		};
	}
}
#endif
