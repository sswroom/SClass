#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Map/ScaledMapView.h"
#include "Math/Math.h"

extern "C"
{
	Bool ScaledMapView_IMapXYToScnXY(const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Double rRate, Double dleft, Double dbottom, Double xmul, Double ymul, Int32 ofstX, Int32 ofstY, UOSInt scnWidth, UOSInt scnHeight);
}

Map::ScaledMapView::ScaledMapView(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale, Bool projected) : Map::MapView(scnSize)
{
	this->projected = projected;
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	ChangeViewXY(scnSize, centMap, scale);
}

Map::ScaledMapView::~ScaledMapView()
{
}

void Map::ScaledMapView::ChangeViewXY(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale)
{
	this->centMap = centMap;
	if (scale < 400)
		scale = 400;
	if (scale > 100000000)
		scale = 100000000;
	this->scale = scale;

	Math::Coord2DDbl diff;
	if (projected)
	{
		diff = scnSize * (0.5 * scale / (this->hdpi * 72.0 / 96.0) * 0.0254);
	}
	else
	{
		diff = scnSize * (0.0000025 * scale / (this->hdpi * 72.0 / 96.0) * 0.0254);
	}
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

void Map::ScaledMapView::UpdateSize(Math::Size2DDbl scnSize)
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

Bool Map::ScaledMapView::MapXYToScnXY(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2D<Int32>> destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const
{
	if (nPoints == 0)
	{
		return false;
	}

	Math::Coord2DDbl mul = this->scnSize / (this->br - this->tl);
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
		destArr[0].x = thisX = Double2Int32((srcArr[0].x - dleft) * mul.x + ofst.x);
		destArr[0].y = thisY = Double2Int32((dbottom - srcArr[0].y) * mul.y + ofst.y);
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
	return (imaxX >= 0) && (iminX < (OSInt)scnSize.x) && (imaxY >= 0) && (iminY < (OSInt)scnSize.y);
}

Bool Map::ScaledMapView::MapXYToScnXY(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UOSInt nPoints, Math::Coord2DDbl ofst) const
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
	Doublex2 ptMul = PDoublex2Set(this->scnSize.x / (this->br.x - this->tl.x), this->scnSize.y / (this->br.y - this->tl.y));
	Doublex2 ptOfst = PDoublex2Set(ofst.x, ofst.y);
	Doublex2 thisVal;
	Doublex2 minVal;
	Doublex2 maxVal;
	thisVal = PDoublex2Set((srcArr[0].x  - dleft), (dbottom - srcArr[0].y));
	minVal = maxVal = PADDPD(PMULPD(thisVal, ptMul), ptOfst);
	PStoreDoublex2((Double*)destArr.Ptr(), minVal);
	srcArr++;
	destArr++;
	nPoints--;
	while (nPoints-- > 0)
	{
		thisVal = PDoublex2Set((srcArr[0].x  - dleft), (dbottom - srcArr[0].y));
		thisVal = PADDPD(PMULPD(thisVal, ptMul), ptOfst);
		PStoreDoublex2((Double*)destArr.Ptr(), thisVal);
		srcArr++;
		destArr++;
		minVal = PMINPD(minVal, thisVal);
		maxVal = PMAXPD(maxVal, thisVal);
	}
	return (Doublex2GetLo(maxVal) >= 0) && (Doublex2GetLo(minVal) < scnSize.x) && (Doublex2GetHi(maxVal) >= 0) && (Doublex2GetHi(minVal) < scnSize.y);
}

Bool Map::ScaledMapView::IMapXYToScnXY(Double mapRate, UnsafeArray<const Math::Coord2D<Int32>> srcArr, UnsafeArray<Math::Coord2D<Int32>> destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const
{
	if (nPoints == 0)
	{
		return false;
	}
	Math::Coord2DDbl mul = this->scnSize / (this->br - this->tl);
	Double dleft = this->tl.x;
	Double dbottom = this->br.y;
	Double rRate = 1 / mapRate;

	return ScaledMapView_IMapXYToScnXY(srcArr.Ptr(), destArr.Ptr(), nPoints, rRate, dleft, dbottom, mul.x, mul.y, ofst.x, ofst.y, (UOSInt)this->scnSize.x, (UOSInt)this->scnSize.y);
}

Math::Coord2DDbl Map::ScaledMapView::MapXYToScnXY(Math::Coord2DDbl mapPos) const
{
	return Math::Coord2DDbl(mapPos.x - this->tl.x, this->br.y - mapPos.y) * scnSize / (this->br - this->tl);
}

Math::Coord2DDbl Map::ScaledMapView::ScnXYToMapXY(Math::Coord2DDbl scnPos) const
{
	Math::Coord2DDbl v = scnPos * (this->br - this->tl) / scnSize;
	return Math::Coord2DDbl(this->tl.x + v.x, this->br.y - v.y);
}

NN<Map::MapView> Map::ScaledMapView::Clone() const
{
	NN<Map::ScaledMapView> view;
	NEW_CLASSNN(view, Map::ScaledMapView(this->scnSize, this->centMap, this->scale, this->projected));
	view->SetDPI(this->hdpi, this->ddpi);
	return view;
}

Double Map::ScaledMapView::CalcScale(Math::RectAreaDbl bounds, Math::Size2DDbl scnSize, Double dpi, Bool projected)
{
	Math::Coord2DDbl diff = bounds.GetSize();
	if (projected)
	{
		diff = diff / 0.0254 * (dpi * 72.0 / 96.0) / scnSize / 1.0;
	}
	else
	{
		diff =  diff / 0.0254 * (dpi * 72.0 / 96.0) / scnSize / 0.000005;
	}
	return (diff.x + diff.y) * 0.5;
}
