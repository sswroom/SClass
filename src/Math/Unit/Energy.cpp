#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Energy.h"

Double Math::Unit::Energy::GetUnitRatio(EnergyUnit unit)
{
	switch (unit)
	{
	case EU_JOULE:
		return 1.0;
	case EU_WATTHOUR:
		return 3600.0;
	case EU_KILOWATTHOUR:
		return 3600000.0;
	}
	return 1;
}

const UTF8Char *Math::Unit::Energy::GetUnitShortName(EnergyUnit unit)
{
	switch (unit)
	{
	case EU_JOULE:
		return (const UTF8Char*)"J";
	case EU_WATTHOUR:
		return (const UTF8Char*)"Wh";
	case EU_KILOWATTHOUR:
		return (const UTF8Char*)"kWh";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Energy::GetUnitName(EnergyUnit unit)
{
	switch (unit)
	{
	case EU_JOULE:
		return (const UTF8Char*)"Joule";
	case EU_WATTHOUR:
		return (const UTF8Char*)"Watt-hour";
	case EU_KILOWATTHOUR:
		return (const UTF8Char*)"Kilowatt-hour";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Energy::Convert(EnergyUnit fromUnit, EnergyUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
