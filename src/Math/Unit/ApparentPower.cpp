#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/ApparentPower.h"

Double Math::Unit::ApparentPower::GetUnitRatio(ApparentPowerUnit unit)
{
	switch (unit)
	{
	case APU_VOLTAMPERE:
		return 1.0;
	}
	return 1;
}

Text::CString Math::Unit::ApparentPower::GetUnitShortName(ApparentPowerUnit unit)
{
	switch (unit)
	{
	case APU_VOLTAMPERE:
		return CSTR("VA");
	}
	return CSTR("");
}

Text::CString Math::Unit::ApparentPower::GetUnitName(ApparentPowerUnit unit)
{
	switch (unit)
	{
	case APU_VOLTAMPERE:
		return CSTR("Volt-Ampere");
	}
	return CSTR("");
}

Double Math::Unit::ApparentPower::GetConvertRatio(ApparentPowerUnit fromUnit, ApparentPowerUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::ApparentPower::Convert(ApparentPowerUnit fromUnit, ApparentPowerUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
