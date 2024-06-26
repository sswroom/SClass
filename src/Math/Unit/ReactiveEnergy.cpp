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

Text::CStringNN Math::Unit::ReactiveEnergy::GetUnitShortName(ReactiveEnergyUnit unit)
{
	switch (unit)
	{
	case REU_KVARH:
		return CSTR("kvarh");
	}
	return CSTR("");
}

Text::CStringNN Math::Unit::ReactiveEnergy::GetUnitName(ReactiveEnergyUnit unit)
{
	switch (unit)
	{
	case REU_KVARH:
		return CSTR("Kilovolt-amperer hour");
	}
	return CSTR("");
}

Double Math::Unit::ReactiveEnergy::GetConvertRatio(ReactiveEnergyUnit fromUnit, ReactiveEnergyUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::ReactiveEnergy::Convert(ReactiveEnergyUnit fromUnit, ReactiveEnergyUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
