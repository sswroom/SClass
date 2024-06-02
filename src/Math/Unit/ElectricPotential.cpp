#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/ElectricPotential.h"

Double Math::Unit::ElectricPotential::GetUnitRatio(ElectricPotentialUnit unit)
{
	switch (unit)
	{
	case EPU_VOLT:
		return 1.0;
	case EPU_MILLIVOLT:
		return 0.001;
	}
	return 1;
}

Text::CStringNN Math::Unit::ElectricPotential::GetUnitShortName(ElectricPotentialUnit unit)
{
	switch (unit)
	{
	case EPU_VOLT:
		return CSTR("V");
	case EPU_MILLIVOLT:
		return CSTR("mV");
	}
	return CSTR("");
}

Text::CStringNN Math::Unit::ElectricPotential::GetUnitName(ElectricPotentialUnit unit)
{
	switch (unit)
	{
	case EPU_VOLT:
		return CSTR("Volt");
	case EPU_MILLIVOLT:
		return CSTR("Millivolt");
	}
	return CSTR("");
}

Double Math::Unit::ElectricPotential::GetConvertRatio(ElectricPotentialUnit fromUnit, ElectricPotentialUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::ElectricPotential::Convert(ElectricPotentialUnit fromUnit, ElectricPotentialUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
