#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Map/MercatorMapView.h"
#include "Math/Math.h"

Map::MercatorMapView::MercatorMapView(Math::Size2D<Double> scnSize, Double centLat, Double centLon, UOSInt maxLevel, UOSInt tileSize) : Map::MapView(scnSize)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	this->centMap.x = centLon;
	this->centMap.y = centLat;
	this->maxLevel = maxLevel;
	this->level = maxLevel >> 1;
	this->dtileSize = UOSInt2Double(tileSize);
	this->UpdateXY();
}

Map::MercatorMapView::~MercatorMapView()
{
}

void Map::MercatorMapView::ChangeViewXY(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale)
{
	this->scnSize = scnSize;
	this->centMap = centMap;
	SetMapScale(scale);
}

void Map::MercatorMapView::SetCenterXY(Math::Coord2DDbl mapPos)
{
	this->centMap = mapPos;
	this->UpdateXY();
}

void Map::MercatorMapView::SetMapScale(Double scale)
{
	OSInt level = (OSInt)(Math_Log10(204094080000.0 / scale / this->dtileSize) / Math_Log10(2));
	if (level < 0)
		level = 0;
	else if (level >= (OSInt)this->maxLevel)
		level = (OSInt)this->maxLevel - 1;
	this->level = (UOSInt)level;
	this->UpdateXY();
}

void Map::MercatorMapView::UpdateSize(Math::Size2D<Double> scnSize)
{
	this->scnSize = scnSize;
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
	this->centPixel.x = Lon2PixelX(this->centMap.x);
	this->centPixel.y = Lat2PixelY(this->centMap.y);
}

Double Map::MercatorMapView::GetLeftX()
{
	return PixelX2Lon(this->centPixel.x - this->scnSize.width * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetTopY()
{
	return PixelY2Lat(this->centPixel.y + this->scnSize.height * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetRightX()
{
	return PixelX2Lon(this->centPixel.x + this->scnSize.width * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetBottomY()
{
	return PixelY2Lat(this->centPixel.y - this->scnSize.height * 0.5 * this->ddpi / this->hdpi);
}

Double Map::MercatorMapView::GetMapScale()
{
	return 204094080000.0 / this->dtileSize / (1 << this->level);
}

Double Map::MercatorMapView::GetViewScale()
{
	return 204094080000.0 / this->dtileSize / (1 << this->level) * this->ddpi / 96.0;
}

Double Map::MercatorMapView::GetCenterX()
{
	return this->centMap.x;
}

Double Map::MercatorMapView::GetCenterY()
{
	return this->centMap.y;
}

Double Map::MercatorMapView::GetHDPI()
{
	return this->hdpi;
}

Double Map::MercatorMapView::GetDDPI()
{
	return this->ddpi;
}

Bool Map::MercatorMapView::InViewXY(Math::Coord2DDbl mapPos)
{
	Double dscale;
	dscale = this->hdpi / this->ddpi;
	Double x = (Lon2PixelX(mapPos.x) - this->centPixel.x) * dscale + this->scnSize.width * 0.5;
	Double y = (Lat2PixelY(mapPos.y) - this->centPixel.y) * dscale + this->scnSize.height * 0.5;
	return x >= 0 && x < this->scnSize.width && y >= 0 && y < this->scnSize.height;
}

Bool Map::MercatorMapView::MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
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
		thisX = Double2Int32((Lon2PixelX(*srcArr++) - this->centPixel.x) * rate + this->scnSize.width * 0.5);
		thisY = Double2Int32((Lat2PixelY(*srcArr++) - this->centPixel.y) * rate + this->scnSize.height * 0.5);
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
	return (imaxX >= 0) && (iminX < (OSInt)scnSize.width) && (imaxY >= 0) && (iminY < (OSInt)scnSize.height);
}

Bool Map::MercatorMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofstPt)
{
	if (nPoints == 0)
	{
		return false;
	}
	
/*	Double iminX = 0;
	Double iminY = 0;
	Double imaxX = 0;
	Double imaxY = 0;
	Double thisX;
	Double thisY;
	Double rate;
	Double dScnWidth = UOSInt2Double(this->scnWidth) * 0.5;
	Double dScnHeight = UOSInt2Double(this->scnHeight) * 0.5;
	rate = this->hdpi / this->ddpi;

	iminX = imaxX = ((Lon2PixelX(*srcArr++) - this->centPixelX) * rate + dScnWidth);
	iminY = imaxY = ((Lat2PixelY(*srcArr++) - this->centPixelY) * rate + dScnHeight);
	*destArr++ = iminX + ofstX;
	*destArr++ = iminY + ofstY;
	nPoints--;
	while (nPoints-- > 0)
	{
		thisX = ((Lon2PixelX(*srcArr++) - this->centPixelX) * rate + dScnWidth);
		thisY = ((Lat2PixelY(*srcArr++) - this->centPixelY) * rate + dScnHeight);
		*destArr++ = thisX + ofstX;
		*destArr++ = thisY + ofstY;
		if (thisX < iminX)
			iminX = thisX;
		if (thisX > imaxX)
			imaxX = thisX;
		if (thisY < iminY)
			iminY = thisY;
		if (thisY > imaxY)
			imaxY = thisY;
	}
	return (imaxX >= 0) && (iminX < UOSInt2Double(scnWidth)) && (imaxY >= 0) && (iminY < UOSInt2Double(scnHeight));*/
	Doublex2 imin;
	Doublex2 imax;
	Doublex2 thisVal;
	Doublex2 rate = PDoublex2SetA(this->hdpi / this->ddpi);
	Doublex2 hScnSize = PDoublex2Set(this->scnSize.width * 0.5, this->scnSize.height * 0.5);
	Doublex2 centPixel = this->centPixel.vals;
	Doublex2 ofst = PDoublex2Set(ofstPt.x, ofstPt.y);
	thisVal = PDoublex2Set(Lon2PixelX(srcArr->x), Lat2PixelY(srcArr->y));
	imin = imax = PADDPD(PMULPD(PSUBPD(thisVal, centPixel), rate), hScnSize);
	PStoreDoublex2((Double*)destArr, PADDPD(imin, ofst));
	srcArr += 1;
	destArr += 1;
	nPoints--;
	while (nPoints-- > 0)
	{
		thisVal = PDoublex2Set(Lon2PixelX(srcArr->x), Lat2PixelY(srcArr->y));
		thisVal = PADDPD(PMULPD(PSUBPD(thisVal, centPixel), rate), hScnSize);
		PStoreDoublex2((Double*)destArr, PADDPD(thisVal, ofst));
		srcArr += 1;
		destArr += 1;
		imin = PMINPD(imin, thisVal);
		imax = PMAXPD(imax, thisVal);
	}
	return (Doublex2GetLo(imax) >= 0) && (Doublex2GetLo(imin) < this->scnSize.width) && (Doublex2GetHi(imax) >= 0) && (Doublex2GetHi(imin) < this->scnSize.height);
}

Bool Map::MercatorMapView::IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
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
	Double dScnWidth = this->scnSize.width * 0.5;
	Double dScnHeight = this->scnSize.height * 0.5;
	*destArr++ = iminX = imaxX = Double2Int32((Lon2PixelX(*srcArr++ * rRate) - this->centPixel.x) * rate + dScnWidth);
	*destArr++ = iminY = imaxY = Double2Int32((Lat2PixelY(*srcArr++ * rRate) - this->centPixel.y) * rate + dScnHeight);
	nPoints--;
	while (nPoints-- > 0)
	{
		*destArr++ = thisX = Double2Int32((Lon2PixelX(*srcArr++ * rRate) - this->centPixel.x) * rate + dScnWidth);
		*destArr++ = thisY = Double2Int32((Lat2PixelY(*srcArr++ * rRate) - this->centPixel.y) * rate + dScnHeight);
		if (thisX < iminX)
			iminX = thisX;
		if (thisX > imaxX)
			imaxX = thisX;
		if (thisY < iminY)
			iminY = thisY;
		if (thisY > imaxY)
			imaxY = thisY;
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnSize.width) && (imaxY >= 0) && (iminY < (OSInt)scnSize.height);
}

Math::Coord2DDbl Map::MercatorMapView::MapXYToScnXY(Math::Coord2DDbl mapPos)
{
	return (Math::Coord2DDbl(Lon2PixelX(mapPos.x), Lat2PixelY(mapPos.y)) - this->centPixel) * this->hdpi / this->ddpi + this->scnSize.ToCoord() * 0.5;
}

Math::Coord2DDbl Map::MercatorMapView::ScnXYToMapXY(Math::Coord2DDbl scnPos)
{
	Math::Coord2DDbl px = (scnPos - this->scnSize.ToCoord() * 0.5) * this->ddpi / this->hdpi + this->centPixel;
	return Math::Coord2DDbl(PixelX2Lon(px.x), PixelY2Lat(px.y));
}

Map::MapView *Map::MercatorMapView::Clone()
{
	Map::MercatorMapView *view;
	NEW_CLASS(view, Map::MercatorMapView(this->scnSize, this->centMap.y, this->centMap.x, this->maxLevel, (UOSInt)this->dtileSize));
	return view;
}

Double Map::MercatorMapView::Lon2PixelX(Double lon)
{
	return ((lon + 180.0) / 360.0 * (1 << this->level)) * this->dtileSize;
}

Double Map::MercatorMapView::Lat2PixelY(Double lat)
{
	return ((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << this->level)) * this->dtileSize;
}

Double Map::MercatorMapView::PixelX2Lon(Double x)
{
	return x / this->dtileSize * 360.0 / (1 << this->level) - 180;
}

Double Map::MercatorMapView::PixelY2Lat(Double y)
{
	Double n = Math::PI - 2.0 * Math::PI * y / this->dtileSize / (1 << level);
	return 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
}
