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

Text::CString Math::Unit::MagneticField::GetUnitShortName(MagneticFieldUnit unit)
{
	switch (unit)
	{
	case MFU_TESLA:
		return CSTR("T");
	case MFU_GAUSS:
		return CSTR("G");
	case MFU_MICROTESLA:
		return CSTR("uT");
	case MFU_MILLITESLA:
		return CSTR("mT");
	}
	return CSTR("");
}

Text::CString Math::Unit::MagneticField::GetUnitName(MagneticFieldUnit unit)
{
	switch (unit)
	{
	case MFU_TESLA:
		return CSTR("Tesla");
	case MFU_GAUSS:
		return CSTR("Gauss");
	case MFU_MICROTESLA:
		return CSTR("Micro Tesla");
	case MFU_MILLITESLA:
		return CSTR("Milli Tesla");
	}
	return CSTR("");
}

Double Math::Unit::MagneticField::Convert(MagneticFieldUnit fromUnit, MagneticFieldUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}
