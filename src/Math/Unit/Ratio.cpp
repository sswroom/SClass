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

Text::CStringNN Math::Unit::Ratio::GetUnitShortName(RatioUnit unit)
{
	switch (unit)
	{
	case RU_RATIO:
		return CSTR("");
	case RU_PERCENT:
		return CSTR("%");
	}
	return CSTR("");
}

Text::CStringNN Math::Unit::Ratio::GetUnitName(RatioUnit unit)
{
	switch (unit)
	{
	case RU_RATIO:
		return CSTR("Ratio");
	case RU_PERCENT:
		return CSTR("Percent");
	}
	return CSTR("");
}

Double Math::Unit::Ratio::GetConvertRatio(RatioUnit fromUnit, RatioUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Ratio::Convert(RatioUnit fromUnit, RatioUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
