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

Text::CString Math::Unit::Ratio::GetUnitShortName(RatioUnit unit)
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

Text::CString Math::Unit::Ratio::GetUnitName(RatioUnit unit)
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

Double Math::Unit::Ratio::Convert(RatioUnit fromUnit, RatioUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
