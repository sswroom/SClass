#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MercatorMapView.h"
#include "Math/Math.h"

Map::MercatorMapView::MercatorMapView(UOSInt scnWidth, UOSInt scnHeight, Double centLat, Double centLon, UOSInt maxLevel, UOSInt tileSize) : Map::MapView(scnWidth, scnHeight)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	this->centMapY = centLat;
	this->centMapX = centLon;
	this->maxLevel = maxLevel;
	this->level = maxLevel >> 1;
	this->tileSize = tileSize;
	this->UpdateXY();
}

Map::MercatorMapView::~MercatorMapView()
{
}

void Map::MercatorMapView::ChangeViewXY(UOSInt scnWidth, UOSInt scnHeight, Double centX, Double centY, Double scale)
{
	this->scnWidth = scnWidth;
	this->scnHeight = scnHeight;
	this->centMapX = centX;
	this->centMapY = centY;
	SetMapScale(scale);
}

void Map::MercatorMapView::SetCenterXY(Double mapX, Double mapY)
{
	this->centMapX = mapX;
	this->centMapY = mapY;
	this->UpdateXY();
}

void Map::MercatorMapView::SetMapScale(Double scale)
{
	OSInt level = (OSInt)(Math::Log10(204094080000.0 / scale / this->tileSize) / Math::Log10(2));
	if (level < 0)
		level = 0;
	else if (level >= (OSInt)this->maxLevel)
		level = this->maxLevel - 1;
	this->level = level;
	this->UpdateXY();
}

void Map::MercatorMapView::UpdateSize(UOSInt width, UOSInt height)
{
	this->scnWidth = width;
	this->scnHeight = height;
}

void Map::MercatorMapView::SetDPI(Double hdpi, Double ddpi)
{
	if (hdpi > 0 && ddpi > 0 && (this->hdpi != hdpi || this->ddpi != ddpi))
	{
		this->hdpi = hdpi;
		this->ddpi = ddpi;
	}
}

void Map::MercatorMapView::UpdateXY()
{
	this->centPixelX = Lon2PixelX(this->centMapX);
	this->centPixelY = Lat2PixelY(this->centMapY);
}

Double Map::MercatorMapView::GetLeftX()
{
	return PixelX2Lon(this->centPixelX - this->scnWidth * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetTopY()
{
	return PixelY2Lat(this->centPixelY + this->scnHeight * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetRightX()
{
	return PixelX2Lon(this->centPixelX + this->scnWidth * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetBottomY()
{
	return PixelY2Lat(this->centPixelY - this->scnHeight * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetMapScale()
{
	return 204094080000.0 / this->tileSize / (1 << this->level);
}

Double Map::MercatorMapView::GetViewScale()
{
	return 204094080000.0 / this->tileSize / (1 << this->level) * this->ddpi / 96.0;
}

Double Map::MercatorMapView::GetCenterX()
{
	return this->centMapX;
}

Double Map::MercatorMapView::GetCenterY()
{
	return this->centMapY;
}

Double Map::MercatorMapView::GetHDPI()
{
	return this->hdpi;
}

Double Map::MercatorMapView::GetDDPI()
{
	return this->ddpi;
}

Bool Map::MercatorMapView::InViewXY(Double mapX, Double mapY)
{
	Double dscale;
	dscale = this->hdpi / this->ddpi;
	Double x = (Lon2PixelX(mapX) - this->centPixelX) * dscale + this->scnWidth * 0.5;
	Double y = (Lat2PixelY(mapY) - this->centPixelY) * dscale + this->scnHeight * 0.5;
	return x >= 0 && x < this->scnWidth && y >= 0 && y < this->scnHeight;
}

Bool Map::MercatorMapView::MapXYToScnXY(const Double *srcArr, Int32 *destArr, OSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}
	Int32 iminX = 0;
	Int32 iminY = 0;
	Int32 imaxX = 0;
	Int32 imaxY = 0;
	Int32 thisX;
	Int32 thisY;
	Double rate;
	rate = this->hdpi / this->ddpi;
	while (nPoints-- > 0)
	{
		thisX = Math::Double2Int32((Lon2PixelX(*srcArr++) - this->centPixelX) * rate + this->scnWidth * 0.5);
		thisY = Math::Double2Int32((Lat2PixelY(*srcArr++) - this->centPixelY) * rate + this->scnHeight * 0.5);
		*destArr++ = thisX + ofstX;
		*destArr++ = thisY + ofstY;
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisX;
			iminY = imaxY = thisY;
		}
		else
		{
			if (thisX < iminX)
				iminX = thisX;
			if (thisX > imaxX)
				imaxX = thisX;
			if (thisY < iminY)
				iminY = thisY;
			if (thisY > imaxY)
				imaxY = thisY;
		}
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnWidth) && (imaxY >= 0) && (iminY < (OSInt)scnHeight);
}

Bool Map::MercatorMapView::MapXYToScnXY(const Double *srcArr, Double *destArr, OSInt nPoints, Double ofstX, Double ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}
	Double iminX = 0;
	Double iminY = 0;
	Double imaxX = 0;
	Double imaxY = 0;
	Double thisX;
	Double thisY;
	Double rate;
	rate = this->hdpi / this->ddpi;
	while (nPoints-- > 0)
	{
		thisX = ((Lon2PixelX(*srcArr++) - this->centPixelX) * rate + this->scnWidth * 0.5);
		thisY = ((Lat2PixelY(*srcArr++) - this->centPixelY) * rate + this->scnHeight * 0.5);
		*destArr++ = thisX + ofstX;
		*destArr++ = thisY + ofstY;
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisX;
			iminY = imaxY = thisY;
		}
		else
		{
			if (thisX < iminX)
				iminX = thisX;
			if (thisX > imaxX)
				imaxX = thisX;
			if (thisY < iminY)
				iminY = thisY;
			if (thisY > imaxY)
				imaxY = thisY;
		}
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnWidth) && (imaxY >= 0) && (iminY < (OSInt)scnHeight);
}

Bool Map::MercatorMapView::IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, OSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}
	Double rRate = 1 / mapRate;
	Int32 iminX = 0;
	Int32 iminY = 0;
	Int32 imaxX = 0;
	Int32 imaxY = 0;
	Int32 thisX;
	Int32 thisY;
	Double rate;
	rate = this->hdpi / this->ddpi;
	while (nPoints-- > 0)
	{
		*destArr++ = thisX = Math::Double2Int32((Lon2PixelX(*srcArr++ * rRate) - this->centPixelX) * rate + this->scnWidth * 0.5);
		*destArr++ = thisY = Math::Double2Int32((Lat2PixelY(*srcArr++ * rRate) - this->centPixelY) * rate + this->scnHeight * 0.5);
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisX;
			iminY = imaxY = thisY;
		}
		else
		{
			if (thisX < iminX)
				iminX = thisX;
			if (thisX > imaxX)
				imaxX = thisX;
			if (thisY < iminY)
				iminY = thisY;
			if (thisY > imaxY)
				imaxY = thisY;
		}
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnWidth) && (imaxY >= 0) && (iminY < (OSInt)scnHeight);
}

void Map::MercatorMapView::MapXYToScnXY(Double mapX, Double mapY, Double *scnX, Double *scnY)
{
	*scnX = (Lon2PixelX(mapX) - this->centPixelX) * this->hdpi / this->ddpi + this->scnWidth * 0.5;
	*scnY = (Lat2PixelY(mapY) - this->centPixelY) * this->hdpi / this->ddpi + this->scnHeight * 0.5;
}

void Map::MercatorMapView::ScnXYToMapXY(Double scnX, Double scnY, Double *mapX, Double *mapY)
{
	*mapX = PixelX2Lon((scnX - this->scnWidth * 0.5) * this->ddpi / this->hdpi + this->centPixelX);
	*mapY = PixelY2Lat((scnY - this->scnHeight * 0.5) * this->ddpi / this->hdpi + this->centPixelY);
}

Map::MapView *Map::MercatorMapView::Clone()
{
	Map::MercatorMapView *view;
	NEW_CLASS(view, Map::MercatorMapView(this->scnWidth, this->scnHeight, this->centMapY, this->centMapX, this->maxLevel, this->tileSize));
	return view;
}

Double Map::MercatorMapView::Lon2PixelX(Double lon)
{
	return ((lon + 180.0) / 360.0 * (1 << this->level)) * this->tileSize;
}

Double Map::MercatorMapView::Lat2PixelY(Double lat)
{
	return ((1.0 - Math::Ln( Math::Tan(lat * Math::PI / 180.0) + 1.0 / Math::Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << this->level)) * this->tileSize;
}

Double Map::MercatorMapView::PixelX2Lon(Double x)
{
	return x / this->tileSize * 360.0 / (1 << this->level) - 180;
}

Double Map::MercatorMapView::PixelY2Lat(Double y)
{
	Double n = Math::PI - 2.0 * Math::PI * y / this->tileSize / (1 << level);
	return 180.0 / Math::PI * Math::ArcTan(0.5 * (Math::Exp(n) - Math::Exp(-n)));
}
