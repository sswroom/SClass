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

const UTF8Char *Math::Unit::Force::GetUnitShortName(ForceUnit unit)
{
	switch (unit)
	{
	case FU_NEWTON:
		return (const UTF8Char*)"N";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Force::GetUnitName(ForceUnit unit)
{
	switch (unit)
	{
	case FU_NEWTON:
		return (const UTF8Char*)"Newton";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Force::Convert(ForceUnit fromUnit, ForceUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
