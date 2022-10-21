#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Force.h"

Double Math::Unit::Force::GetUnitRatio(ForceUnit unit)
{
	switch (unit)
	{
	case FU_NEWTON:
		return 1.0;
	}
	return 1;
}

Text::CString Math::Unit::Force::GetUnitShortName(ForceUnit unit)
{
	switch (unit)
	{
	case FU_NEWTON:
		return CSTR("N");
	}
	return CSTR("");
}

Text::CString Math::Unit::Force::GetUnitName(ForceUnit unit)
{
	switch (unit)
	{
	case FU_NEWTON:
		return CSTR("Newton");
	}
	return CSTR("");
}

Double Math::Unit::Force::GetConvertRatio(ForceUnit fromUnit, ForceUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Force::Convert(ForceUnit fromUnit, ForceUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
