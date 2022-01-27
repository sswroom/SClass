#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/ElectricCurrent.h"

Double Math::Unit::ElectricCurrent::GetUnitRatio(ElectricCurrentUnit unit)
{
	switch (unit)
	{
	case ECU_AMPERE:
		return 1.0;
	case ECU_MILLIAMPERE:
		return 0.001;
	case ECU_MICROAMPERE:
		return 0.000001;
	}
	return 1;
}

Text::CString Math::Unit::ElectricCurrent::GetUnitShortName(ElectricCurrentUnit unit)
{
	switch (unit)
	{
	case ECU_AMPERE:
		return CSTR("A");
	case ECU_MILLIAMPERE:
		return CSTR("mA");
	case ECU_MICROAMPERE:
		return CSTR("uA");
	}
	return CSTR("");
}

Text::CString Math::Unit::ElectricCurrent::GetUnitName(ElectricCurrentUnit unit)
{
	switch (unit)
	{
	case ECU_AMPERE:
		return CSTR("Ampere");
	case ECU_MILLIAMPERE:
		return CSTR("Milliampere");
	case ECU_MICROAMPERE:
		return CSTR("Microampere");
	}
	return CSTR("");
}

Double Math::Unit::ElectricCurrent::Convert(ElectricCurrentUnit fromUnit, ElectricCurrentUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
