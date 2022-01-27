#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Math/Unit/Speed.h"

Double Math::Unit::Speed::GetUnitRatio(SpeedUnit unit)
{
	switch (unit)
	{
	case SU_METER_PER_SECOND:
		return 1.0;
	case SU_KM_PER_HOUR:
		return 1000 / 3600.0;
	case SU_MILE_PER_HOUR:
		return Math::Unit::Distance::GetUnitRatio(Math::Unit::Distance::DU_MILE) / 3600.0;
	case SU_KNOT:
		return Math::Unit::Distance::GetUnitRatio(Math::Unit::Distance::DU_NAUTICAL_MILE) / 3600.0;
	}
	return 1;
}


Text::CString Math::Unit::Speed::GetUnitShortName(SpeedUnit unit)
{
	switch (unit)
	{
	case SU_METER_PER_SECOND:
		return CSTR("m/s");
	case SU_KM_PER_HOUR:
		return CSTR("km/h");
	case SU_MILE_PER_HOUR:
		return CSTR("mph");
	case SU_KNOT:
		return CSTR("knot");
	}
	return CSTR("");
}

Text::CString Math::Unit::Speed::GetUnitName(SpeedUnit unit)
{
	switch (unit)
	{
	case SU_METER_PER_SECOND:
		return CSTR("meter per second");
	case SU_KM_PER_HOUR:
		return CSTR("kilometer per hour");
	case SU_MILE_PER_HOUR:
		return CSTR("mile per hour");
	case SU_KNOT:
		return CSTR("knot");
	}
	return CSTR("");
}

Double Math::Unit::Speed::Convert(SpeedUnit fromUnit, SpeedUnit toUnit, Double fromValue)
{
	return (fromValue * GetUnitRatio(fromUnit)) / GetUnitRatio(toUnit);
}

Double Math::Unit::Speed::CalcDryAirSpeed(Double temperatureK, SpeedUnit unit)
{
	return Convert(Math::Unit::Speed::SU_METER_PER_SECOND, unit, 20.05 * Math_Sqrt(temperatureK));
}
