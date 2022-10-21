#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/ReactivePower.h"

Double Math::Unit::ReactivePower::GetUnitRatio(ReactivePowerUnit unit)
{
	switch (unit)
	{
	case RPU_VAR:
		return 1.0;
	}
	return 1;
}

Text::CString Math::Unit::ReactivePower::GetUnitShortName(ReactivePowerUnit unit)
{
	switch (unit)
	{
	case RPU_VAR:
		return CSTR("var");
	}
	return CSTR("");
}

Text::CString Math::Unit::ReactivePower::GetUnitName(ReactivePowerUnit unit)
{
	switch (unit)
	{
	case RPU_VAR:
		return CSTR("Volt-Ampere Reactive");
	}
	return CSTR("");
}

Double Math::Unit::ReactivePower::GetConvertRatio(ReactivePowerUnit fromUnit, ReactivePowerUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::ReactivePower::Convert(ReactivePowerUnit fromUnit, ReactivePowerUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
