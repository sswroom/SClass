#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Energy.h"

Double Math::Unit::Energy::GetUnitRatio(EnergyUnit unit)
{
	switch (unit)
	{
	case EU_JOULE:
		return 1.0;
	case EU_WATTHOUR:
		return 3600.0;
	case EU_KILOWATTHOUR:
		return 3600000.0;
	case EU_CALORIE:
		return 4.168;
	case EU_KILOCALORIE:
		return 4168.0;
	}
	return 1;
}

Text::CString Math::Unit::Energy::GetUnitShortName(EnergyUnit unit)
{
	switch (unit)
	{
	case EU_JOULE:
		return CSTR("J");
	case EU_WATTHOUR:
		return CSTR("Wh");
	case EU_KILOWATTHOUR:
		return CSTR("kWh");
	case EU_CALORIE:
		return CSTR("cal");
	case EU_KILOCALORIE:
		return CSTR("kcal");
	}
	return CSTR("");
}

Text::CString Math::Unit::Energy::GetUnitName(EnergyUnit unit)
{
	switch (unit)
	{
	case EU_JOULE:
		return CSTR("Joule");
	case EU_WATTHOUR:
		return CSTR("Watt-hour");
	case EU_KILOWATTHOUR:
		return CSTR("Kilowatt-hour");
	case EU_CALORIE:
		return CSTR("Calorie");
	case EU_KILOCALORIE:
		return CSTR("Kilocalorie");
	}
	return CSTR("");
}

Double Math::Unit::Energy::GetConvertRatio(EnergyUnit fromUnit, EnergyUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Energy::Convert(EnergyUnit fromUnit, EnergyUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
