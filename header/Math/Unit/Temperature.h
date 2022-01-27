#ifndef _SM_MATH_UNIT_TEMPERATURE
#define _SM_MATH_UNIT_TEMPERATURE
#include "Text/CString.h"

namespace Math
{
	namespace Unit
	{
		class Temperature
		{
		public:
			typedef enum
			{
				TU_CELSIUS,
				TU_KELVIN,
				TU_FAHRENHEIT
			} TemperatureUnit;
		public:
			static Double GetUnitRatio(TemperatureUnit unit);
			static Double GetUnitScale(TemperatureUnit unit);
			static Text::CString GetUnitShortName(TemperatureUnit unit);
			static Text::CString GetUnitName(TemperatureUnit unit);
			static Double Convert(TemperatureUnit fromUnit, TemperatureUnit toUnit, Double fromValue);
		};
	}
}
#endif
