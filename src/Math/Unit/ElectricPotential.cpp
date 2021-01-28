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

const UTF8Char *Math::Unit::ElectricPotential::GetUnitShortName(ElectricPotentialUnit unit)
{
	switch (unit)
	{
	case EPU_VOLT:
		return (const UTF8Char*)"V";
	case EPU_MILLIVOLT:
		return (const UTF8Char*)"mV";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::ElectricPotential::GetUnitName(ElectricPotentialUnit unit)
{
	switch (unit)
	{
	case EPU_VOLT:
		return (const UTF8Char*)"Volt";
	case EPU_MILLIVOLT:
		return (const UTF8Char*)"Millivolt";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::ElectricPotential::Convert(ElectricPotentialUnit fromUnit, ElectricPotentialUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
