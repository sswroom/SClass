#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Power.h"

Double Math::Unit::Power::GetUnitRatio(PowerUnit unit)
{
	switch (unit)
	{
	case PU_WATT:
		return 1.0;
	case PU_MILLIVOLT:
		return 0.001;
	case PU_KILOVOLT:
		return 1000.0;
	}
	return 1;
}

const UTF8Char *Math::Unit::Power::GetUnitShortName(PowerUnit unit)
{
	switch (unit)
	{
	case PU_WATT:
		return (const UTF8Char*)"W";
	case PU_MILLIVOLT:
		return (const UTF8Char*)"mW";
	case PU_KILOVOLT:
		return (const UTF8Char*)"kW";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Power::GetUnitName(PowerUnit unit)
{
	switch (unit)
	{
	case PU_WATT:
		return (const UTF8Char*)"Watt";
	case PU_MILLIVOLT:
		return (const UTF8Char*)"Milliwatt";
	case PU_KILOVOLT:
		return (const UTF8Char*)"Kilowatt";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Power::Convert(PowerUnit fromUnit, PowerUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
