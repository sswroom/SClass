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
	}
	return CSTR("");
}

Double Math::Unit::Energy::Convert(EnergyUnit fromUnit, EnergyUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
