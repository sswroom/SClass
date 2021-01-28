#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/ElectricCurrent.h"

Double Math::Unit::ElectricCurrent::GetUnitRatio(ElectricCurrentUnit unit)
{
	switch (unit)
	{
	case ECU_AMPERE:
		return 1.0;
	case ECU_MILLIAMPERE:
		return 0.001;
	case ECU_MICROAMPERE:
		return 0.000001;
	}
	return 1;
}

const UTF8Char *Math::Unit::ElectricCurrent::GetUnitShortName(ElectricCurrentUnit unit)
{
	switch (unit)
	{
	case ECU_AMPERE:
		return (const UTF8Char*)"A";
	case ECU_MILLIAMPERE:
		return (const UTF8Char*)"mA";
	case ECU_MICROAMPERE:
		return (const UTF8Char*)"uA";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::ElectricCurrent::GetUnitName(ElectricCurrentUnit unit)
{
	switch (unit)
	{
	case ECU_AMPERE:
		return (const UTF8Char*)"Ampere";
	case ECU_MILLIAMPERE:
		return (const UTF8Char*)"Milliampere";
	case ECU_MICROAMPERE:
		return (const UTF8Char*)"Microampere";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::ElectricCurrent::Convert(ElectricCurrentUnit fromUnit, ElectricCurrentUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
