#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/ReactiveEnergy.h"

Double Math::Unit::ReactiveEnergy::GetUnitRatio(ReactiveEnergyUnit unit)
{
	switch (unit)
	{
	case REU_KVARH:
		return 1.0;
	}
	return 1;
}

const UTF8Char *Math::Unit::ReactiveEnergy::GetUnitShortName(ReactiveEnergyUnit unit)
{
	switch (unit)
	{
	case REU_KVARH:
		return (const UTF8Char*)"kvarh";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::ReactiveEnergy::GetUnitName(ReactiveEnergyUnit unit)
{
	switch (unit)
	{
	case REU_KVARH:
		return (const UTF8Char*)"Kilovolt-amperer hour";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::ReactiveEnergy::Convert(ReactiveEnergyUnit fromUnit, ReactiveEnergyUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
