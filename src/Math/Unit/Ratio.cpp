#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Ratio.h"

Double Math::Unit::Ratio::GetUnitRatio(RatioUnit unit)
{
	switch (unit)
	{
	case RU_RATIO:
		return 1.0;
	case RU_PERCENT:
		return 0.01;
	}
	return 1;
}

const UTF8Char *Math::Unit::Ratio::GetUnitShortName(RatioUnit unit)
{
	switch (unit)
	{
	case RU_RATIO:
		return (const UTF8Char*)"";
	case RU_PERCENT:
		return (const UTF8Char*)"%";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Ratio::GetUnitName(RatioUnit unit)
{
	switch (unit)
	{
	case RU_RATIO:
		return (const UTF8Char*)"Ratio";
	case RU_PERCENT:
		return (const UTF8Char*)"Percent";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Ratio::Convert(RatioUnit fromUnit, RatioUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
