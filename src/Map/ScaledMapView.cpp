#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Map/ScaledMapView.h"
#include "Math/Math.h"

extern "C"
{
	Bool ScaledMapView_IMapXYToScnXY(const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Double rRate, Double dleft, Double dbottom, Double xmul, Double ymul, Int32 ofstX, Int32 ofstY, UOSInt scnWidth, UOSInt scnHeight);
}

Map::ScaledMapView::ScaledMapView(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale) : Map::MapView(scnSize)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	ChangeViewXY(scnSize, centMap, scale);
}

Map::ScaledMapView::~ScaledMapView()
{
}

void Map::ScaledMapView::ChangeViewXY(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale)
{
	this->centMap = centMap;
	if (scale < 400)
		scale = 400;
	if (scale > 100000000)
		scale = 100000000;
	this->scale = scale;

	Math::Coord2DDbl diff = scnSize.ToCoord() * (0.00025 * scale / (this->hdpi * 72.0 / 96.0) * 2.54 / 10000.0);
	this->tl = centMap - diff;
	this->br = centMap + diff;
	this->scnSize = scnSize;
}

void Map::ScaledMapView::SetCenterXY(Math::Coord2DDbl mapPos)
{
	ChangeViewXY(this->scnSize, mapPos, scale);
}

void Map::ScaledMapView::SetMapScale(Double scale)
{
	ChangeViewXY(this->scnSize, this->centMap, scale);
}

void Map::ScaledMapView::UpdateSize(Math::Size2D<Double> scnSize)
{
	ChangeViewXY(scnSize, this->centMap, scale);
}

void Map::ScaledMapView::SetDPI(Double hdpi, Double ddpi)
{
	if (hdpi > 0 && ddpi > 0 && (this->hdpi != hdpi || this->ddpi != ddpi))
	{
		this->hdpi = hdpi;
		this->ddpi = ddpi;
		ChangeViewXY(this->scnSize, this->centMap, this->scale);
	}
}

Math::Quadrilateral Map::ScaledMapView::GetBounds() const
{
	return Math::Quadrilateral(this->tl, Math::Coord2DDbl(this->br.x, this->tl.y), this->br, Math::Coord2DDbl(this->tl.x, this->br.y));
}

Math::RectAreaDbl Map::ScaledMapView::GetVerticalRect() const
{
	Math::Coord2DDbl sz = this->br - this->tl;
	return Math::RectAreaDbl(this->tl.x, this->tl.y, sz.x, sz.y);
}

Double Map::ScaledMapView::GetMapScale() const
{
	return this->scale;
}

Double Map::ScaledMapView::GetViewScale() const
{
	return this->scale;
}

Math::Coord2DDbl Map::ScaledMapView::GetCenter() const
{
	return this->centMap;
}

Double Map::ScaledMapView::GetHDPI() const
{
	return this->hdpi;
}

Double Map::ScaledMapView::GetDDPI() const
{
	return this->ddpi;
}

Bool Map::ScaledMapView::InViewXY(Math::Coord2DDbl mapPos) const
{
	return mapPos >= this->tl && mapPos < this->br;
}

Bool Map::ScaledMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const
{
	if (nPoints == 0)
	{
		return false;
	}

	Math::Coord2DDbl mul = this->scnSize.ToCoord() / (this->br - this->tl);
	Double dleft = this->tl.x;
	Double dbottom = this->br.y;
	Int32 iminX = 0;
	Int32 iminY = 0;
	Int32 imaxX = 0;
	Int32 imaxY = 0;
	Int32 thisX;
	Int32 thisY;
	while (nPoints-- > 0)
	{
		destArr->x = thisX = Double2Int32((srcArr->x - dleft) * mul.x + ofst.x);
		destArr->y = thisY = Double2Int32((dbottom - srcArr->y) * mul.y + ofst.y);
		srcArr++;
		destArr++;
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

Bool Map::ScaledMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst) const
{
	if (nPoints == 0)
	{
		return false;
	}

/*	Double xmul = this->scnWidth / (rightX - leftX);
	Double ymul = this->scnHeight / (bottomY - topY);
	Double dleft = leftX;
	Double dbottom = bottomY;
	Double iminX = 0;
	Double iminY = 0;
	Double imaxX = 0;
	Double imaxY = 0;
	Double thisX;
	Double thisY;
	while (nPoints-- > 0)
	{
		*destArr++ = thisX = ((*srcArr++  - dleft) * xmul + ofstX);
		*destArr++ = thisY = ((dbottom - *srcArr++) * ymul + ofstY);
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
	return (imaxX >= 0) && (iminX < scnWidth) && (imaxY >= 0) && (iminY < scnHeight);*/

	Double dleft = this->tl.x;
	Double dbottom = this->br.y;
	Doublex2 ptMul = PDoublex2Set(this->scnSize.width / (this->br.x - this->tl.x), this->scnSize.height / (this->br.y - this->tl.y));
	Doublex2 ptOfst = PDoublex2Set(ofst.x, ofst.y);
	Doublex2 thisVal;
	Doublex2 minVal;
	Doublex2 maxVal;
	thisVal = PDoublex2Set((srcArr->x  - dleft), (dbottom - srcArr->y));
	minVal = maxVal = PADDPD(PMULPD(thisVal, ptMul), ptOfst);
	PStoreDoublex2((Double*)destArr, minVal);
	srcArr++;
	destArr++;
	nPoints--;
	while (nPoints-- > 0)
	{
		thisVal = PDoublex2Set((srcArr->x  - dleft), (dbottom - srcArr->y));
		thisVal = PADDPD(PMULPD(thisVal, ptMul), ptOfst);
		PStoreDoublex2((Double*)destArr, thisVal);
		srcArr++;
		destArr++;
		minVal = PMINPD(minVal, thisVal);
		maxVal = PMAXPD(maxVal, thisVal);
	}
	return (Doublex2GetLo(maxVal) >= 0) && (Doublex2GetLo(minVal) < scnSize.width) && (Doublex2GetHi(maxVal) >= 0) && (Doublex2GetHi(minVal) < scnSize.height);
}

Bool Map::ScaledMapView::IMapXYToScnXY(Double mapRate, const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const
{
	if (nPoints == 0)
	{
		return false;
	}
	Math::Coord2DDbl mul = this->scnSize.ToCoord() / (this->br - this->tl);
	Double dleft = this->tl.x;
	Double dbottom = this->br.y;
	Double rRate = 1 / mapRate;

	return ScaledMapView_IMapXYToScnXY(srcArr, destArr, nPoints, rRate, dleft, dbottom, mul.x, mul.y, ofst.x, ofst.y, (UOSInt)this->scnSize.width, (UOSInt)this->scnSize.height);
}

Math::Coord2DDbl Map::ScaledMapView::MapXYToScnXY(Math::Coord2DDbl mapPos) const
{
	return Math::Coord2DDbl(mapPos.x - this->tl.x, this->br.y - mapPos.y) * scnSize.ToCoord() / (this->br - this->tl);
}

Math::Coord2DDbl Map::ScaledMapView::ScnXYToMapXY(Math::Coord2DDbl scnPos) const
{
	Math::Coord2DDbl v = scnPos * (this->br - this->tl) / scnSize.ToCoord();
	return Math::Coord2DDbl(this->tl.x + v.x, this->br.y - v.y);
}

Map::MapView *Map::ScaledMapView::Clone() const
{
	Map::ScaledMapView *view;
	NEW_CLASS(view, Map::ScaledMapView(this->scnSize, this->centMap, this->scale));
	return view;
}
