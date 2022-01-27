#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Count.h"

Double Math::Unit::Count::GetUnitRatio(CountUnit unit)
{
	switch (unit)
	{
	case CU_UNIT:
		return 1.0;
	}
	return 1;
}

Text::CString Math::Unit::Count::GetUnitShortName(CountUnit unit)
{
	switch (unit)
	{
	case CU_UNIT:
		return CSTR("");
	}
	return CSTR("");
}

Text::CString Math::Unit::Count::GetUnitName(CountUnit unit)
{
	switch (unit)
	{
	case CU_UNIT:
		return CSTR("");
	}
	return CSTR("");
}

Double Math::Unit::Count::Convert(CountUnit fromUnit, CountUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
