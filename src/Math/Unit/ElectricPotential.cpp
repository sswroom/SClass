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

Text::CString Math::Unit::ElectricPotential::GetUnitShortName(ElectricPotentialUnit unit)
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

Text::CString Math::Unit::ElectricPotential::GetUnitName(ElectricPotentialUnit unit)
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

Double Math::Unit::ElectricPotential::Convert(ElectricPotentialUnit fromUnit, ElectricPotentialUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
