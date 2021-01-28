#ifndef _SM_MATH_UNIT_PRESSURE
#define _SM_MATH_UNIT_PRESSURE
#include "Math/Unit/Substance.h"
#include "Math/Unit/Temperature.h"
namespace Math
{
	namespace Unit
	{
		class Pressure
		{
		public:
			typedef enum
			{
				PU_PASCAL,
				PU_BAR,
				PU_ATM,
				PU_TORR,
				PU_PSI,
				PU_KPASCAL,
				PU_HPASCAL
			} PressureUnit;
		public:
			static Double GetUnitRatio(PressureUnit unit);
			static const UTF8Char *GetUnitShortName(PressureUnit unit);
			static const UTF8Char *GetUnitName(PressureUnit unit);
			static Double Convert(PressureUnit fromUnit, PressureUnit toUnit, Double fromValue);

			static Double VapourPressureAntoine(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue, Math::Unit::Substance::SubstanceType substance); // Antoine equation
			static Double WaterVapourPressureTetens(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue); //Tetens equation
			static Double WaterVapourPressureBuck(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue); //Buck equation
			static Double WaterVapourPressure(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue, Double rh);
		};
	}
}
#endif
