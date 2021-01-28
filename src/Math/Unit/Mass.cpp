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

const UTF8Char *Math::Unit::Mass::GetUnitShortName(MassUnit unit)
{
	switch (unit)
	{
	case MU_KILOGRAM:
		return (const UTF8Char*)"kg";
	case MU_GRAM:
		return (const UTF8Char*)"g";
	case MU_TONNE:
		return (const UTF8Char*)"t";
	case MU_POUND:
		return (const UTF8Char*)"lb";
	case MU_OZ:
		return (const UTF8Char*)"oz";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Mass::GetUnitName(MassUnit unit)
{
	switch (unit)
	{
	case MU_KILOGRAM:
		return (const UTF8Char*)"Kilogram";
	case MU_GRAM:
		return (const UTF8Char*)"Gram";
	case MU_TONNE:
		return (const UTF8Char*)"Tonne";
	case MU_POUND:
		return (const UTF8Char*)"Pounds";
	case MU_OZ:
		return (const UTF8Char*)"Ounce";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Mass::Convert(MassUnit fromUnit, MassUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
