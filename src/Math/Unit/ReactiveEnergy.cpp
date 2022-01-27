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

Text::CString Math::Unit::ReactiveEnergy::GetUnitShortName(ReactiveEnergyUnit unit)
{
	switch (unit)
	{
	case REU_KVARH:
		return CSTR("kvarh");
	}
	return CSTR("");
}

Text::CString Math::Unit::ReactiveEnergy::GetUnitName(ReactiveEnergyUnit unit)
{
	switch (unit)
	{
	case REU_KVARH:
		return CSTR("Kilovolt-amperer hour");
	}
	return CSTR("");
}

Double Math::Unit::ReactiveEnergy::Convert(ReactiveEnergyUnit fromUnit, ReactiveEnergyUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
