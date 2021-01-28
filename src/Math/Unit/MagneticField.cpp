#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/MagneticField.h"

Double Math::Unit::MagneticField::GetUnitRatio(MagneticFieldUnit unit)
{
	switch (unit)
	{
	case MFU_TESLA:
		return 1;
	case MFU_GAUSS:
		return 0.0001;
	case MFU_MICROTESLA:
		return 0.000001;
	case MFU_MILLITESLA:
		return 0.001;
	}
	return 1;
}

const UTF8Char *Math::Unit::MagneticField::GetUnitShortName(MagneticFieldUnit unit)
{
	switch (unit)
	{
	case MFU_TESLA:
		return (const UTF8Char*)"T";
	case MFU_GAUSS:
		return (const UTF8Char*)"G";
	case MFU_MICROTESLA:
		return (const UTF8Char*)"uT";
	case MFU_MILLITESLA:
		return (const UTF8Char*)"mT";
	}
	return (const UTF8Char*)"";
}

const UTF8Char *Math::Unit::MagneticField::GetUnitName(MagneticFieldUnit unit)
{
	switch (unit)
	{
	case MFU_TESLA:
		return (const UTF8Char*)"Tesla";
	case MFU_GAUSS:
		return (const UTF8Char*)"Gauss";
	case MFU_MICROTESLA:
		return (const UTF8Char*)"Micro Tesla";
	case MFU_MILLITESLA:
		return (const UTF8Char*)"Milli Tesla";
	}
	return (const UTF8Char*)"";
}

Double Math::Unit::MagneticField::Convert(MagneticFieldUnit fromUnit, MagneticFieldUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
