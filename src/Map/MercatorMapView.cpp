#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Map/MercatorMapView.h"
#include "Math/Math_C.h"

Map::MercatorMapView::MercatorMapView(Math::Size2DDbl scnSize, Math::Coord2DDbl center, UOSInt maxLevel, UOSInt tileSize) : Map::MapView(scnSize)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	this->centMap = center;
	this->maxLevel = maxLevel;
	this->level = maxLevel >> 1;
	this->dtileSize = UOSInt2Double(tileSize);
	this->UpdateXY();
}

Map::MercatorMapView::~MercatorMapView()
{
}

void Map::MercatorMapView::ChangeViewXY(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale)
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
	if (scale < 10)
	{
		scale = 10;
	}
	OSInt level = (OSInt)(Math_Log10(204094080000.0 / scale / this->dtileSize) / Math_Log10(2));
	if (level < 0)
		level = 0;
//	else if (level >= (OSInt)this->maxLevel)
//		level = (OSInt)this->maxLevel - 1;
	this->level = (UOSInt)level;
	this->UpdateXY();
}

void Map::MercatorMapView::UpdateSize(Math::Size2DDbl scnSize)
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

Math::Quadrilateral Map::MercatorMapView::GetBounds() const
{
	return this->GetVerticalRect().Rotate(this->hAngle);
}

Math::RectAreaDbl Map::MercatorMapView::GetVerticalRect() const
{
	Double left = PixelX2Lon(this->centPixel.x - this->scnSize.x * 0.5 * this->ddpi / this->hdpi);
	Double top = PixelY2Lat(this->centPixel.y + this->scnSize.y * 0.5 * this->ddpi / this->hdpi);
	Double right = PixelX2Lon(this->centPixel.x + this->scnSize.x * 0.5 * this->ddpi / this->hdpi);
	Double bottom = PixelY2Lat(this->centPixel.y - this->scnSize.y * 0.5 * this->ddpi / this->hdpi);
	return Math::RectAreaDbl(left, top, right - left, bottom - top);
}

Double Map::MercatorMapView::GetMapScale() const
{
	return 204094080000.0 / this->dtileSize / (1 << this->level);
}

Double Map::MercatorMapView::GetViewScale() const
{
	return 204094080000.0 / this->dtileSize / (1 << this->level) * this->ddpi / 96.0;
}

Math::Coord2DDbl Map::MercatorMapView::GetCenter() const
{
	return this->centMap;
}

Double Map::MercatorMapView::GetHDPI() const
{
	return this->hdpi;
}

Double Map::MercatorMapView::GetDDPI() const
{
	return this->ddpi;
}

Bool Map::MercatorMapView::InViewXY(Math::Coord2DDbl mapPos) const
{
	Double dscale;
	dscale = this->hdpi / this->ddpi;
	Double x = (Lon2PixelX(mapPos.x) - this->centPixel.x) * dscale + this->scnSize.x * 0.5;
	Double y = (Lat2PixelY(mapPos.y) - this->centPixel.y) * dscale + this->scnSize.y * 0.5;
	return x >= 0 && x < this->scnSize.x && y >= 0 && y < this->scnSize.y;
}

Bool Map::MercatorMapView::MapXYToScnXYArr(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UOSInt nPoints, Math::Coord2DDbl ofstPt) const
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
	Doublex2 hScnSize = PDoublex2Set(this->scnSize.x * 0.5, this->scnSize.y * 0.5);
	Doublex2 centPixel = this->centPixel.vals;
	Doublex2 ofst = PDoublex2Set(ofstPt.x, ofstPt.y);
	thisVal = PDoublex2Set(Lon2PixelX(srcArr[0].x), Lat2PixelY(srcArr[0].y));
	imin = imax = PADDPD(PMULPD(PSUBPD(thisVal, centPixel), rate), hScnSize);
	if (this->hAngle == 0)
	{
		PStoreDoublex2((Double*)destArr.Ptr(), PADDPD(imin, ofst));
		srcArr += 1;
		destArr += 1;
		nPoints--;
		while (nPoints-- > 0)
		{
			thisVal = PDoublex2Set(Lon2PixelX(srcArr[0].x), Lat2PixelY(srcArr[0].y));
			thisVal = PADDPD(PMULPD(PSUBPD(thisVal, centPixel), rate), hScnSize);
			PStoreDoublex2((Double*)destArr.Ptr(), PADDPD(thisVal, ofst));
			srcArr += 1;
			destArr += 1;
			imin = PMINPD(imin, thisVal);
			imax = PMAXPD(imax, thisVal);
		}
	}
	else
	{
		Doublex2 rotXMul = PDoublex2Set(this->hICos, this->hISin);
		Doublex2 rotYMul = PDoublex2Set(-this->hISin, this->hICos);
		Doublex2 scnCenter = (this->scnSize * 0.5).vals;
		Doublex2 diff = PSUBPD(imin, scnCenter);
		PStoreDoublex2((Double*)destArr.Ptr(), PADDPD(PADDPD(scnCenter, HADDPD(PMULPD(diff, rotXMul), PMULPD(diff, rotYMul))), ofst));
		srcArr += 1;
		destArr += 1;
		nPoints--;
		while (nPoints-- > 0)
		{
			thisVal = PDoublex2Set(Lon2PixelX(srcArr[0].x), Lat2PixelY(srcArr[0].y));
			thisVal = PADDPD(PMULPD(PSUBPD(thisVal, centPixel), rate), hScnSize);
			diff = PSUBPD(thisVal, scnCenter);
			PStoreDoublex2((Double*)destArr.Ptr(), PADDPD(PADDPD(scnCenter, HADDPD(PMULPD(diff, rotXMul), PMULPD(diff, rotYMul))), ofst));
			srcArr += 1;
			destArr += 1;
			imin = PMINPD(imin, thisVal);
			imax = PMAXPD(imax, thisVal);
		}
	}
	return (Doublex2GetLo(imax) >= 0) && (Doublex2GetLo(imin) < this->scnSize.x) && (Doublex2GetHi(imax) >= 0) && (Doublex2GetHi(imin) < this->scnSize.y);
}

Bool Map::MercatorMapView::IMapXYToScnXY(Double mapRate, UnsafeArray<const Math::Coord2D<Int32>> srcArr, UnsafeArray<Math::Coord2D<Int32>> destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const
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
	Double dScnWidth = this->scnSize.x * 0.5;
	Double dScnHeight = this->scnSize.y * 0.5;
	destArr[0].x = iminX = imaxX = Double2Int32((Lon2PixelX(srcArr[0].x * rRate) - this->centPixel.x) * rate + dScnWidth);
	destArr[0].y = iminY = imaxY = Double2Int32((Lat2PixelY(srcArr[0].y * rRate) - this->centPixel.y) * rate + dScnHeight);
	srcArr++;
	destArr++;
	nPoints--;
	while (nPoints-- > 0)
	{
		destArr[0].x = thisX = Double2Int32((Lon2PixelX(srcArr[0].x * rRate) - this->centPixel.x) * rate + dScnWidth);
		destArr[0].y = thisY = Double2Int32((Lat2PixelY(srcArr[0].y * rRate) - this->centPixel.y) * rate + dScnHeight);
		srcArr++;
		destArr++;
		if (thisX < iminX)
			iminX = thisX;
		if (thisX > imaxX)
			imaxX = thisX;
		if (thisY < iminY)
			iminY = thisY;
		if (thisY > imaxY)
			imaxY = thisY;
	}
	return (imaxX >= 0) && (iminX < (OSInt)scnSize.x) && (imaxY >= 0) && (iminY < (OSInt)scnSize.y);
}

Math::Coord2DDbl Map::MercatorMapView::MapXYToScnXYNoDir(Math::Coord2DDbl mapPos) const
{
	return (Math::Coord2DDbl(Lon2PixelX(mapPos.x), Lat2PixelY(mapPos.y)) - this->centPixel) * this->hdpi / this->ddpi + this->scnSize * 0.5;
}

Math::Coord2DDbl Map::MercatorMapView::ScnXYNoDirToMapXY(Math::Coord2DDbl scnPos) const
{
	Math::Coord2DDbl px = (scnPos - this->scnSize * 0.5) * this->ddpi / this->hdpi + this->centPixel;
	return Math::Coord2DDbl(PixelX2Lon(px.x), PixelY2Lat(px.y));
}

NN<Map::MapView> Map::MercatorMapView::Clone() const
{
	NN<Map::MercatorMapView> view;
	NEW_CLASSNN(view, Map::MercatorMapView(this->scnSize, this->centMap, this->maxLevel, (UOSInt)this->dtileSize));
	return view;
}

Double Map::MercatorMapView::Lon2PixelX(Double lon) const
{
	return ((lon + 180.0) / 360.0 * (1 << this->level)) * this->dtileSize;
}

Double Map::MercatorMapView::Lat2PixelY(Double lat) const
{
	return ((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << this->level)) * this->dtileSize;
}

Double Map::MercatorMapView::PixelX2Lon(Double x) const
{
	return x / this->dtileSize * 360.0 / (1 << this->level) - 180;
}

Double Map::MercatorMapView::PixelY2Lat(Double y) const
{
	Double n = Math::PI - 2.0 * Math::PI * y / this->dtileSize / (1 << level);
	return 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
}
