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
		return 1.8;
	}
	return 1;
}

Double Math::Unit::Temperature::GetUnitScale(TemperatureUnit unit)
{
	switch (unit)
	{
	case TU_CELSIUS:
		return 273.15;
	case TU_KELVIN:
		return 0.0;
	case TU_FAHRENHEIT:
		return 459.67;
	}
	return 1;
}

const UTF8Char *Math::Unit::Temperature::GetUnitShortName(TemperatureUnit unit)
{
	static const UTF8Char c[] = {0xE2, 0x84, 0x83, 0};
	static const UTF8Char f[] = {0xE2, 0x84, 0x89, 0};
	switch (unit)
	{
	case TU_CELSIUS:
		return c;
	case TU_KELVIN:
		return (const UTF8Char*)"K";
	case TU_FAHRENHEIT:
		return f;
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Temperature::GetUnitName(TemperatureUnit unit)
{
	switch (unit)
	{
	case TU_CELSIUS:
		return (const UTF8Char*)"Degree Celsius";
	case TU_KELVIN:
		return (const UTF8Char*)"Kelvin";
	case TU_FAHRENHEIT:
		return (const UTF8Char*)"Degree Fahrenheit";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Temperature::Convert(TemperatureUnit fromUnit, TemperatureUnit toUnit, Double fromValue)
{
	return (fromValue * GetUnitRatio(fromUnit) - GetUnitScale(fromUnit)) / GetUnitRatio(toUnit) + GetUnitScale(toUnit);
}
