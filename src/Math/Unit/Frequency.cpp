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

Text::CString Math::Unit::Frequency::GetUnitShortName(FrequencyUnit unit)
{
	switch (unit)
	{
	case FU_HERTZ:
		return CSTR("Hz");
	case FU_KILOHERTZ:
		return CSTR("kHz");
	}
	return CSTR("");
}

Text::CString Math::Unit::Frequency::GetUnitName(FrequencyUnit unit)
{
	switch (unit)
	{
	case FU_HERTZ:
		return CSTR("Hertz");
	case FU_KILOHERTZ:
		return CSTR("Kilohertz");
	}
	return CSTR("");
}

Double Math::Unit::Frequency::Convert(FrequencyUnit fromUnit, FrequencyUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
