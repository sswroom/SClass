#ifndef _SM_MATH_UNIT_FORCE
#define _SM_MATH_UNIT_FORCE
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Force
		{
		public:
			typedef enum
			{
				FU_NEWTON
			} ForceUnit;
		public:
			static Double GetUnitRatio(ForceUnit unit);
			static Text::CString GetUnitShortName(ForceUnit unit);
			static Text::CString GetUnitName(ForceUnit unit);
			static Double Convert(ForceUnit fromUnit, ForceUnit toUnit, Double fromValue);
		};
	}
}
#endif
