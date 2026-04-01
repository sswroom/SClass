#include "Stdafx.h"
#include "Media/Image.h"

Text::CStringNN Media::ImageTypeGetName(ImageType imgType)
{
	switch (imgType)
	{
	case ImageType::Raster:
		return CSTR("Raster");
	case ImageType::Vector:
		return CSTR("Vector");
	case ImageType::SVG:
		return CSTR("SVG");
	default:
		return CSTR("Unknown");
	}
}
