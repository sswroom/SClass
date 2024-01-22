#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Power.h"

Double Math::Unit::Power::GetUnitRatio(PowerUnit unit)
{
	switch (unit)
	{
	case PU_WATT:
		return 1.0;
	case PU_MILLIWATT:
		return 0.001;
	case PU_KILOWATT:
		return 1000.0;
	}
	return 1;
}

Text::CString Math::Unit::Power::GetUnitShortName(PowerUnit unit)
{
	switch (unit)
	{
	case PU_WATT:
		return CSTR("W");
	case PU_MILLIWATT:
		return CSTR("mW");
	case PU_KILOWATT:
		return CSTR("kW");
	}
	return CSTR("");
}

Text::CString Math::Unit::Power::GetUnitName(PowerUnit unit)
{
	switch (unit)
	{
	case PU_WATT:
		return CSTR("Watt");
	case PU_MILLIWATT:
		return CSTR("Milliwatt");
	case PU_KILOWATT:
		return CSTR("Kilowatt");
	}
	return CSTR("");
}

Double Math::Unit::Power::GetConvertRatio(PowerUnit fromUnit, PowerUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Power::Convert(PowerUnit fromUnit, PowerUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
