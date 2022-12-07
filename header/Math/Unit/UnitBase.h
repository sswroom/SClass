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
			enum class ValueType
			{
				Mapping,
				ElectricPotential,
				ElectricCurrent,
				Power,
				Angle,
				Frequency,
				Ratio,
				ApparentPower,
				ReactivePower,
				Energy,
				ReactiveEnergy,
				Count,
				Temperature
			};
		public:
			static Text::CString GetUnitShortName(ValueType vt, Int32 unit);
		};
	}
}
#endif
