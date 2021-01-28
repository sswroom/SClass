#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Angle.h"

Double Math::Unit::Angle::GetUnitRatio(AngleUnit unit)
{
	switch (unit)
	{
	case AU_RADIAN:
		return 1;
	case AU_GRADIAN:
		return Math::PI / 200.0;
	case AU_TURN:
		return Math::PI * 2.0;
	case AU_DEGREE:
		return Math::PI / 180.0;
	case AU_ARCMINUTE:
		return Math::PI / 10800.0;
	case AU_ARCSECOND:
		return Math::PI / 648000.0;
	case AU_MILLIARCSECOND:
		return Math::PI / 648000000.0;
	case AU_MICROARCSECOND:
		return Math::PI / 648000000000.0;
	}
	return 1;
}

const UTF8Char *Math::Unit::Angle::GetUnitShortName(AngleUnit unit)
{
	switch (unit)
	{
	case AU_RADIAN:
		return (const UTF8Char*)"rad";
	case AU_GRADIAN:
		return (const UTF8Char*)"grad";
	case AU_TURN:
		return (const UTF8Char*)"";
	case AU_DEGREE:
	{
		static const UTF8Char ret[] = {0xc2, 0xb0, 0};
		return ret;
	}
	case AU_ARCMINUTE:
	{
		static const UTF8Char ret[] = {0xe2, 0x80, 0xb2, 0};
		return ret;
	}
	case AU_ARCSECOND:
	{
		static const UTF8Char ret[] = {0xe2, 0x80, 0xb3, 0};
		return ret;
	}
	case AU_MILLIARCSECOND:
		return (const UTF8Char*)"mas";
	case AU_MICROARCSECOND:
	{
		static const UTF8Char ret[] = {0xce, 0xbc, 0x61, 0x73, 0};
		return ret;
	}
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Angle::GetUnitName(AngleUnit unit)
{
	switch (unit)
	{
	case AU_RADIAN:
		return (const UTF8Char*)"Radian";
	case AU_GRADIAN:
		return (const UTF8Char*)"Gradian";
	case AU_TURN:
		return (const UTF8Char*)"Turns";
	case AU_DEGREE:
		return (const UTF8Char*)"Degree";
	case AU_ARCMINUTE:
		return (const UTF8Char*)"Arcminute";
	case AU_ARCSECOND:
		return (const UTF8Char*)"Arcsecond";
	case AU_MILLIARCSECOND:
		return (const UTF8Char*)"Milliarcsecond";
	case AU_MICROARCSECOND:
		return (const UTF8Char*)"Microarcsecond";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Angle::Convert(AngleUnit fromUnit, AngleUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
