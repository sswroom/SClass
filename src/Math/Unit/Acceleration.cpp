#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Acceleration.h"

Double Math::Unit::Acceleration::GetUnitRatio(AccelerationUnit unit)
{
	switch (unit)
	{
	case AccelerationUnit::MeterPerSecondSquared:
		return 1;
	case AccelerationUnit::StandardGravity:
		return 9.80665;
	}
	return 1;
}

Text::CStringNN Math::Unit::Acceleration::GetUnitShortName(AccelerationUnit unit)
{
	switch (unit)
	{
	case AccelerationUnit::MeterPerSecondSquared:
		return CSTR("m/s^2");
	case AccelerationUnit::StandardGravity:
		return CSTR("g");
	}
	return CSTR("");
}

Text::CStringNN Math::Unit::Acceleration::GetUnitName(AccelerationUnit unit)
{
	switch (unit)
	{
	case AccelerationUnit::MeterPerSecondSquared:
		return CSTR("Meter Per Second Squared");
	case AccelerationUnit::StandardGravity:
		return CSTR("Gravity");
	}
	return CSTR("");
}

Double Math::Unit::Acceleration::GetConvertRatio(AccelerationUnit fromUnit, AccelerationUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Acceleration::Convert(AccelerationUnit fromUnit, AccelerationUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
