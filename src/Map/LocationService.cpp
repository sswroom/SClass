#include "Stdafx.h"
#include "Map/LocationService.h"

Text::CStringNN Map::LocationService::SateTypeGetName(SateType sateType)
{
	switch (sateType)
	{
	case SateType::GPS:
		return CSTR("GPS");
	case SateType::GLONASS:
		return CSTR("GLONASS");
	case SateType::Galileo:
		return CSTR("Galileo");
	case SateType::QZSS:
		return CSTR("QZSS");
	case SateType::BeiDou:
		return CSTR("BeiDou");
	default:
		return CSTR("Unknown");
	}
}