#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Distance.h"

Double Math::Unit::Distance::GetUnitRatio(DistanceUnit unit)
{
	switch (unit)
	{
	case DU_METER:
		return 1.0;
	case DU_CENTIMETER:
		return 0.01;
	case DU_MILLIMETER:
		return 0.001;
	case DU_MICROMETER:
		return 0.000001;
	case DU_NANOMETER:
		return 0.000000001;
	case DU_PICOMETER:
		return 0.000000000001;
	case DU_KILOMETER:
		return 1000.0;
	case DU_INCH:
		return 0.0254;
	case DU_FOOT:
		return 0.0254 * 12.0;
	case DU_YARD:
		return 0.0254 * 36.0;
	case DU_MILE:
		return 0.0254 * 12.0 * 5280;
	case DU_NAUTICAL_MILE:
		return 1852.0;
	case DU_AU:
		return 149597870700.0;
	case DU_LIGHTSECOND:
		return 299792458.0;
	case DU_LIGHTMINUTE:
		return 17987547480.0;
	case DU_LIGHTHOUR:
		return 299792458.0 * 3600.0;
	case DU_LIGHTDAY:
		return 299792458.0 * 86400.0;
	case DU_LIGHTWEEK:
		return 299792458.0 * 604800.0;
	case DU_LIGHTYEAR:
		return 299792458.0 * 31557600.0;
	case DU_EMU:
		return 1 / 36000000.0;
	case DU_POINT:
		return 0.0254 / 72.0;
	case DU_PIXEL:
		return 0.0254 / 96.0;
	case DU_TWIP:
		return 0.0254 / 1440.0;
	case DU_CLARKE_FOOT:
		return 0.3047972654;
	}
	return 1;
}

Text::CStringNN Math::Unit::Distance::GetUnitShortName(DistanceUnit unit)
{
	switch (unit)
	{
	case DU_METER:
		return CSTR("m");
	case DU_CENTIMETER:
		return CSTR("cm");
	case DU_MILLIMETER:
		return CSTR("mm");
	case DU_MICROMETER:
		return CSTR("μm");
	case DU_NANOMETER:
		return CSTR("nm");
	case DU_PICOMETER:
		return CSTR("pm");
	case DU_KILOMETER:
		return CSTR("km");
	case DU_INCH:
		return CSTR("\"");
	case DU_FOOT:
		return CSTR("ft");
	case DU_YARD:
		return CSTR("yd");
	case DU_MILE:
		return CSTR("mile");
	case DU_NAUTICAL_MILE:
		return CSTR("NM");
	case DU_AU:
		return CSTR("AU");
	case DU_LIGHTSECOND:
		return CSTR("ls");
	case DU_LIGHTMINUTE:
		return CSTR("lm");
	case DU_LIGHTHOUR:
		return CSTR("lh");
	case DU_LIGHTDAY:
		return CSTR("ld");
	case DU_LIGHTWEEK:
		return CSTR("lw");
	case DU_LIGHTYEAR:
		return CSTR("ly");
	case DU_EMU:
		return CSTR("emu");
	case DU_POINT:
		return CSTR("pt");
	case DU_PIXEL:
		return CSTR("px");
	case DU_TWIP:
		return CSTR("twip");
	case DU_CLARKE_FOOT:
		return CSTR("ft(Cla)");
	}
	return CSTR("");
}

Text::CStringNN Math::Unit::Distance::GetUnitName(DistanceUnit unit)
{
	switch (unit)
	{
	case DU_METER:
		return CSTR("Meter");
	case DU_CENTIMETER:
		return CSTR("Centimeter");
	case DU_MILLIMETER:
		return CSTR("Millimeter");
	case DU_MICROMETER:
		return CSTR("Micrometer");
	case DU_NANOMETER:
		return CSTR("Nanometer");
	case DU_PICOMETER:
		return CSTR("Picometer");
	case DU_KILOMETER:
		return CSTR("Kilometer");
	case DU_INCH:
		return CSTR("Inch");
	case DU_FOOT:
		return CSTR("Foot");
	case DU_YARD:
		return CSTR("Yard");
	case DU_MILE:
		return CSTR("Mile");
	case DU_NAUTICAL_MILE:
		return CSTR("Nautical Mile");
	case DU_AU:
		return CSTR("Astronomical unit");
	case DU_LIGHTSECOND:
		return CSTR("Light-second");
	case DU_LIGHTMINUTE:
		return CSTR("Light-minute");
	case DU_LIGHTHOUR:
		return CSTR("Light-hour");
	case DU_LIGHTDAY:
		return CSTR("Light-day");
	case DU_LIGHTWEEK:
		return CSTR("Light-week");
	case DU_LIGHTYEAR:
		return CSTR("Light-year");
	case DU_EMU:
		return CSTR("English Metric Unit");
	case DU_POINT:
		return CSTR("Point");
	case DU_PIXEL:
		return CSTR("Pixel");
	case DU_TWIP:
		return CSTR("Twentieth of an inch point");
	case DU_CLARKE_FOOT:
		return CSTR("Clarke's Foot");
	}
	return CSTR("");
}

Double Math::Unit::Distance::GetConvertRatio(DistanceUnit fromUnit, DistanceUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Distance::Convert(DistanceUnit fromUnit, DistanceUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}

