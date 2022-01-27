#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Mass.h"

Double Math::Unit::Mass::GetUnitRatio(MassUnit unit)
{
	switch (unit)
	{
	case MU_KILOGRAM:
		return 1.0;
	case MU_GRAM:
		return 0.001;
	case MU_TONNE:
		return 1000.0;
	case MU_POUND:
		return 0.45359237;
	case MU_OZ:
		return 0.45359237 / 16;
	}
	return 1;
}

Text::CString Math::Unit::Mass::GetUnitShortName(MassUnit unit)
{
	switch (unit)
	{
	case MU_KILOGRAM:
		return CSTR("kg");
	case MU_GRAM:
		return CSTR("g");
	case MU_TONNE:
		return CSTR("t");
	case MU_POUND:
		return CSTR("lb");
	case MU_OZ:
		return CSTR("oz");
	}
	return CSTR("");
}

Text::CString Math::Unit::Mass::GetUnitName(MassUnit unit)
{
	switch (unit)
	{
	case MU_KILOGRAM:
		return CSTR("Kilogram");
	case MU_GRAM:
		return CSTR("Gram");
	case MU_TONNE:
		return CSTR("Tonne");
	case MU_POUND:
		return CSTR("Pounds");
	case MU_OZ:
		return CSTR("Ounce");
	}
	return CSTR("");
}

Double Math::Unit::Mass::Convert(MassUnit fromUnit, MassUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
