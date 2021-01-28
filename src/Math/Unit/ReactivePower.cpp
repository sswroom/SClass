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

const UTF8Char *Math::Unit::ReactivePower::GetUnitShortName(ReactivePowerUnit unit)
{
	switch (unit)
	{
	case RPU_VAR:
		return (const UTF8Char*)"var";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::ReactivePower::GetUnitName(ReactivePowerUnit unit)
{
	switch (unit)
	{
	case RPU_VAR:
		return (const UTF8Char*)"Volt-Ampere Reactive";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::ReactivePower::Convert(ReactivePowerUnit fromUnit, ReactivePowerUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
