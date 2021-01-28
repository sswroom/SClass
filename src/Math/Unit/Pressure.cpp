#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Pressure.h"

Double Math::Unit::Pressure::GetUnitRatio(PressureUnit unit)
{
	switch (unit)
	{
	case PU_PASCAL:
		return 1.0;
	case PU_BAR:
		return 100000.0;
	case PU_ATM:
		return 101325.0;
	case PU_TORR:
		return 101325.0 / 760.0;
	case PU_PSI:
		return 4.4482216152605 / 0.0254 / 0.0254;
	case PU_KPASCAL:
		return 1000.0;
	case PU_HPASCAL:
		return 100.0;
	}
	return 1;
}

const UTF8Char *Math::Unit::Pressure::GetUnitShortName(PressureUnit unit)
{
	switch (unit)
	{
	case PU_PASCAL:
		return (const UTF8Char*)"Pa";
	case PU_BAR:
		return (const UTF8Char*)"bar";
	case PU_ATM:
		return (const UTF8Char*)"atm";
	case PU_TORR:
		return (const UTF8Char*)"Torr";
	case PU_PSI:
		return (const UTF8Char*)"psi";
	case PU_KPASCAL:
		return (const UTF8Char*)"kPa";
	case PU_HPASCAL:
		return (const UTF8Char*)"hPa";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Pressure::GetUnitName(PressureUnit unit)
{
	switch (unit)
	{
	case PU_PASCAL:
		return (const UTF8Char*)"Pascal";
	case PU_BAR:
		return (const UTF8Char*)"Bar";
	case PU_ATM:
		return (const UTF8Char*)"Standard atmosphere";
	case PU_TORR:
		return (const UTF8Char*)"Torr";
	case PU_PSI:
		return (const UTF8Char*)"Pounds per square inch";
	case PU_KPASCAL:
		return (const UTF8Char*)"Kilo Pascal";
	case PU_HPASCAL:
		return (const UTF8Char*)"Hecto Pascal";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Pressure::Convert(PressureUnit fromUnit, PressureUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Pressure::VapourPressureAntoine(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue, Math::Unit::Substance::SubstanceType substance)
{
	Double deg = Math::Unit::Temperature::Convert(tempUnit, Math::Unit::Temperature::TU_CELSIUS, tempValue);
	Double valA;
	Double valB;
	Double valC;
	switch (substance)
	{
	case Math::Unit::Substance::ST_WATER:
		if (deg >= 1 && deg < 100)
		{
			valA = 8.07131;
			valB = 1730.63;
			valC = 233.426;
		}
		else if (deg >= 100 && deg <= 374)
		{
			valA = 8.14019;
			valB = 1810.94;
			valC = 244.485;
		}
		else
		{
			return 0;
		}
		break;
	case Math::Unit::Substance::ST_ETHANOL:
		if (deg >= -57 && deg <= 80)
		{
			valA = 8.20417;
			valB = 1642.89;
			valC = 230.300;
		}
		else if (deg >= 77 && deg <= 243)
		{
			valA = 7.68117;
			valB = 1332.04;
			valC = 199.200;
		}
		else
		{
			return 0;
		}
		break;
	default:
		return 0;
	}
	return Convert(PU_TORR, outputUnit, Math::Pow(10, valA - valB / (valC + deg)));
}

Double Math::Unit::Pressure::WaterVapourPressureTetens(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue)
{
	Double deg = Math::Unit::Temperature::Convert(tempUnit, Math::Unit::Temperature::TU_CELSIUS, tempValue);
	Double v;
	if (deg < 0)
	{
		v = 0.61078 * Math::Exp((21.875 * deg) / (deg + 265.5));
	}
	else
	{
		v = 0.61078 * Math::Exp((17.27 * deg) / (deg + 237.3));
	}
	return Convert(PU_KPASCAL, outputUnit, v);
}

Double Math::Unit::Pressure::WaterVapourPressureBuck(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue)
{
	Double deg = Math::Unit::Temperature::Convert(tempUnit, Math::Unit::Temperature::TU_CELSIUS, tempValue);
	Double v;
	if (deg < 0)
	{
		v = 0.61115 * Math::Exp((23.036 - deg / 333.7) * (deg / (279.82 + deg)));
	}
	else
	{
		v = 0.61121 * Math::Exp((18.678 - deg / 234.5) * (deg / (257.14 + deg)));
	}
	return Convert(PU_KPASCAL, outputUnit, v);
}

Double Math::Unit::Pressure::WaterVapourPressure(PressureUnit outputUnit, Math::Unit::Temperature::TemperatureUnit tempUnit, Double tempValue, Double rh)
{
	return WaterVapourPressureBuck(outputUnit, tempUnit, tempValue) * rh * 0.01;
}
