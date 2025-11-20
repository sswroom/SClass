#include "Stdafx.h"
#include "Data/ArrayList.hpp"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "Media/JPEGExif.h"
#include "Map/Geotag.h"

Bool Map::Geotag::SetDouble(Int32 *buff, Double val)
{
	Bool isNeg;
	if (val < 0)
	{
		val = -val;
		isNeg = true;
	}
	else
	{
		isNeg = false;
	}
	buff[0] = (Int32)val;
	buff[1] = 1;
	val = (val - buff[0]) * 60;
	buff[2] = (Int32)val;
	buff[3] = 1;
	val = (val - buff[0]) * 60 * 16777216;
	buff[4] = (Int32)val;
	buff[5] = 16777216;
	return isNeg;
}

void Map::Geotag::SetPosition(Media::JPEGExif *exif, Double lat, Double lon)
{
	Int32 buff[6];
	Media::JPEGExif::ExifValue *grp = exif->AddExifGroup(0x8825);
	buff[0] = 0x202;
	exif->SetExif(grp, 0, (UInt8*)&buff[0], 4);
	if (SetDouble(buff, lat))
	{
		exif->SetExif(grp, 1, "S");
	}
	else
	{
		exif->SetExif(grp, 1, "N");
	}
	exif->SetExif(grp, 2, buff, 3);
	if (SetDouble(buff, lon))
	{
		exif->SetExif(grp, 3, "W");
	}
	else
	{
		exif->SetExif(grp, 3, "E");
	}
	exif->SetExif(grp, 4, buff, 3);
	exif->SetExif(grp, 18, "WGS 1984");
}
