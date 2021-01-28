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
	}
	return 1;
}

const UTF8Char *Math::Unit::Distance::GetUnitShortName(DistanceUnit unit)
{
	switch (unit)
	{
	case DU_METER:
		return (const UTF8Char*)"m";
	case DU_CENTIMETER:
		return (const UTF8Char*)"cm";
	case DU_MILLIMETER:
		return (const UTF8Char*)"mm";
	case DU_MICROMETER:
		return (const UTF8Char*)"μm";
	case DU_NANOMETER:
		return (const UTF8Char*)"nm";
	case DU_PICOMETER:
		return (const UTF8Char*)"pm";
	case DU_KILOMETER:
		return (const UTF8Char*)"km";
	case DU_INCH:
		return (const UTF8Char*)"\"";
	case DU_FOOT:
		return (const UTF8Char*)"ft";
	case DU_YARD:
		return (const UTF8Char*)"yd";
	case DU_MILE:
		return (const UTF8Char*)"milw";
	case DU_NAUTICAL_MILE:
		return (const UTF8Char*)"nm";
	case DU_AU:
		return (const UTF8Char*)"AU";
	case DU_LIGHTSECOND:
		return (const UTF8Char*)"ls";
	case DU_LIGHTMINUTE:
		return (const UTF8Char*)"lm";
	case DU_LIGHTHOUR:
		return (const UTF8Char*)"lh";
	case DU_LIGHTDAY:
		return (const UTF8Char*)"ld";
	case DU_LIGHTWEEK:
		return (const UTF8Char*)"lw";
	case DU_LIGHTYEAR:
		return (const UTF8Char*)"ly";
	case DU_EMU:
		return (const UTF8Char*)"emu";
	case DU_POINT:
		return (const UTF8Char*)"pt";
	case DU_PIXEL:
		return (const UTF8Char*)"px";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::Distance::GetUnitName(DistanceUnit unit)
{
	switch (unit)
	{
	case DU_METER:
		return (const UTF8Char*)"Meter";
	case DU_CENTIMETER:
		return (const UTF8Char*)"Centimeter";
	case DU_MILLIMETER:
		return (const UTF8Char*)"Millimeter";
	case DU_MICROMETER:
		return (const UTF8Char*)"Micrometer";
	case DU_NANOMETER:
		return (const UTF8Char*)"Nanometer";
	case DU_PICOMETER:
		return (const UTF8Char*)"Picometer";
	case DU_KILOMETER:
		return (const UTF8Char*)"Kilometer";
	case DU_INCH:
		return (const UTF8Char*)"Inch";
	case DU_FOOT:
		return (const UTF8Char*)"Foot";
	case DU_YARD:
		return (const UTF8Char*)"Yard";
	case DU_MILE:
		return (const UTF8Char*)"Mile";
	case DU_NAUTICAL_MILE:
		return (const UTF8Char*)"Nautical Mile";
	case DU_AU:
		return (const UTF8Char*)"Astronomical unit";
	case DU_LIGHTSECOND:
		return (const UTF8Char*)"Light-second";
	case DU_LIGHTMINUTE:
		return (const UTF8Char*)"Light-minute";
	case DU_LIGHTHOUR:
		return (const UTF8Char*)"Light-hour";
	case DU_LIGHTDAY:
		return (const UTF8Char*)"Light-day";
	case DU_LIGHTWEEK:
		return (const UTF8Char*)"Light-week";
	case DU_LIGHTYEAR:
		return (const UTF8Char*)"Light-year";
	case DU_EMU:
		return (const UTF8Char*)"English Metric Unit";
	case DU_POINT:
		return (const UTF8Char*)"Point";
	case DU_PIXEL:
		return (const UTF8Char*)"Pixel";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::Distance::Convert(DistanceUnit fromUnit, DistanceUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
