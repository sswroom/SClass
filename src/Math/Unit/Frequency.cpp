#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Frequency.h"

Double Math::Unit::Frequency::GetUnitRatio(FrequencyUnit unit)
{
	switch (unit)
	{
	case FU_HERTZ:
		return 1.0;
	case FU_KILOHERTZ:
		return 1000.0;
	}
	return 1;
}

const UTF8Char *Math::Unit::Frequency::GetUnitShortName(FrequencyUnit unit)
{
	switch (unit)
	{
	case FU_HERTZ:
		return (const UTF8Char*)"Hz";
	case FU_KILOHERTZ:
		return (const UTF8Char*)"kHz";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Frequency::GetUnitName(FrequencyUnit unit)
{
	switch (unit)
	{
	case FU_HERTZ:
		return (const UTF8Char*)"Hertz";
	case FU_KILOHERTZ:
		return (const UTF8Char*)"Kilohertz";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Frequency::Convert(FrequencyUnit fromUnit, FrequencyUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
