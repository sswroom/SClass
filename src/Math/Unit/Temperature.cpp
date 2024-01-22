#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Temperature.h"

Double Math::Unit::Temperature::GetUnitRatio(TemperatureUnit unit)
{
	switch (unit)
	{
	case TU_CELSIUS:
		return 1.0;
	case TU_KELVIN:
		return 1.0;
	case TU_FAHRENHEIT:
		return 5.0/9.0;
	}
	return 1;
}

Double Math::Unit::Temperature::GetUnitScale(TemperatureUnit unit)
{
	switch (unit)
	{
	case TU_CELSIUS:
		return -273.15;
	case TU_KELVIN:
		return 0.0;
	case TU_FAHRENHEIT:
		return -459.67;
	}
	return 1;
}

Text::CString Math::Unit::Temperature::GetUnitShortName(TemperatureUnit unit)
{
	static const UTF8Char c[] = {0xE2, 0x84, 0x83, 0};
	static const UTF8Char f[] = {0xE2, 0x84, 0x89, 0};
	switch (unit)
	{
	case TU_CELSIUS:
		return Text::CString(c, 3);
	case TU_KELVIN:
		return CSTR("K");
	case TU_FAHRENHEIT:
		return Text::CString(f, 3);
	}
	return CSTR("");
}

Text::CString Math::Unit::Temperature::GetUnitName(TemperatureUnit unit)
{
	switch (unit)
	{
	case TU_CELSIUS:
		return CSTR("Degree Celsius");
	case TU_KELVIN:
		return CSTR("Kelvin");
	case TU_FAHRENHEIT:
		return CSTR("Degree Fahrenheit");
	}
	return CSTR("");
}

Double Math::Unit::Temperature::Convert(TemperatureUnit fromUnit, TemperatureUnit toUnit, Double fromValue)
{
	return (fromValue * GetUnitRatio(fromUnit) - GetUnitScale(fromUnit)) / GetUnitRatio(toUnit) + GetUnitScale(toUnit);
}
