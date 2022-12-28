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

Text::CString Math::Unit::Angle::GetUnitShortName(AngleUnit unit)
{
	switch (unit)
	{
	case AU_RADIAN:
		return CSTR("rad");
	case AU_GRADIAN:
		return CSTR("grad");
	case AU_TURN:
		return CSTR("");
	case AU_DEGREE:
	{
		static const UTF8Char ret[] = {0xc2, 0xb0, 0};
		return Text::CString(ret, 2);
	}
	case AU_ARCMINUTE:
	{
		static const UTF8Char ret[] = {0xe2, 0x80, 0xb2, 0};
		return Text::CString(ret, 3);
	}
	case AU_ARCSECOND:
	{
		static const UTF8Char ret[] = {0xe2, 0x80, 0xb3, 0};
		return Text::CString(ret, 3);
	}
	case AU_MILLIARCSECOND:
		return CSTR("mas");
	case AU_MICROARCSECOND:
	{
		static const UTF8Char ret[] = {0xce, 0xbc, 0x61, 0x73, 0};
		return Text::CString(ret, 4);
	}
	}
	return CSTR("");
}

Text::CString Math::Unit::Angle::GetUnitName(AngleUnit unit)
{
	switch (unit)
	{
	case AU_RADIAN:
		return CSTR("Radian");
	case AU_GRADIAN:
		return CSTR("Gradian");
	case AU_TURN:
		return CSTR("Turns");
	case AU_DEGREE:
		return CSTR("Degree");
	case AU_ARCMINUTE:
		return CSTR("Arcminute");
	case AU_ARCSECOND:
		return CSTR("Arcsecond");
	case AU_MILLIARCSECOND:
		return CSTR("Milliarcsecond");
	case AU_MICROARCSECOND:
		return CSTR("Microarcsecond");
	}
	return CSTR("");
}

Double Math::Unit::Angle::GetConvertRatio(AngleUnit fromUnit, AngleUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Angle::Convert(AngleUnit fromUnit, AngleUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}
