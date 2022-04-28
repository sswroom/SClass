#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/ProjectedMapView.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"

Map::ProjectedMapView::ProjectedMapView(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale) : Map::MapView(scnSize)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	ChangeViewXY(scnSize, centMap, scale);
}

Map::ProjectedMapView::~ProjectedMapView()
{
}

void Map::ProjectedMapView::ChangeViewXY(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale)
{
	this->centMap = centMap;
	if (scale < 400)
		scale = 400;
	if (scale > 100000000)
		scale = 100000000;
	this->scale = scale;

	Math::Coord2DDbl diff = Math::Coord2DDbl(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, scnSize.width * 0.5 / this->hdpi * this->scale),
		Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, scnSize.height * 0.5 / this->hdpi * this->scale));

	this->tl = centMap - diff;
	this->br = centMap + diff;
	this->scnSize = scnSize;
}

void Map::ProjectedMapView::SetCenterXY(Math::Coord2DDbl mapPos)
{
	ChangeViewXY(this->scnSize, mapPos, scale);
}

void Map::ProjectedMapView::SetMapScale(Double scale)
{
	ChangeViewXY(this->scnSize, this->centMap, scale);
}

void Map::ProjectedMapView::UpdateSize(Math::Size2D<Double> scnSize)
{
	ChangeViewXY(scnSize, this->centMap, this->scale);
}

void Map::ProjectedMapView::SetDPI(Double hdpi, Double ddpi)
{
	if (hdpi > 0 && ddpi > 0 && (this->hdpi != hdpi || this->ddpi != ddpi))
	{
		this->hdpi = hdpi;
		this->ddpi = ddpi;
		ChangeViewXY(this->scnSize, this->centMap, this->scale);
	}
}

Math::Quadrilateral Map::ProjectedMapView::GetBounds()
{
	return Math::Quadrilateral(this->tl, Math::Coord2DDbl(this->br.x, this->tl.y), this->br, Math::Coord2DDbl(this->tl.x, this->br.y));
}

Math::RectAreaDbl Map::ProjectedMapView::GetVerticalRect()
{
	Math::Coord2DDbl sz = this->br - this->tl;
	return Math::RectAreaDbl(this->tl.x, this->tl.y, sz.x, sz.y);
}

Double Map::ProjectedMapView::GetMapScale()
{
	return this->scale;
}

Double Map::ProjectedMapView::GetViewScale()
{
	return this->scale;
}

Math::Coord2DDbl Map::ProjectedMapView::GetCenter()
{
	return this->centMap;
}

Double Map::ProjectedMapView::GetHDPI()
{
	return this->hdpi;
}

Double Map::ProjectedMapView::GetDDPI()
{
	return this->ddpi;
}

Bool Map::ProjectedMapView::InViewXY(Math::Coord2DDbl mapPos)
{
	return mapPos >= this->tl && mapPos < this->br;
}

Bool Map::ProjectedMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
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
		*destArr++ = thisX = Double2Int32((srcArr->x  - dleft) * mul.x + ofstX);
		*destArr++ = thisY = Double2Int32((dbottom - srcArr->y) * mul.y + ofstY);
		srcArr++;
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

Bool Map::ProjectedMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst)
{
	if (nPoints == 0)
	{
		return false;
	}

	Math::Coord2DDbl mul = this->scnSize.ToCoord() / (this->br - this->tl);
	Double dleft = this->tl.x;
	Double dbottom = this->br.y;
	Double iminX = 0;
	Double iminY = 0;
	Double imaxX = 0;
	Double imaxY = 0;
	Math::Coord2DDbl thisPt;
	while (nPoints-- > 0)
	{
		destArr[0] = thisPt = Math::Coord2DDbl(srcArr->x - dleft, dbottom - srcArr->y) * mul + ofst;
		destArr++;
		srcArr++;
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisPt.x;
			iminY = imaxY = thisPt.y;
		}
		else
		{
			if (thisPt.x < iminX)
				iminX = thisPt.x;
			if (thisPt.x > imaxX)
				imaxX = thisPt.x;
			if (thisPt.y < iminY)
				iminY = thisPt.y;
			if (thisPt.y > imaxY)
				imaxY = thisPt.y;
		}
	}
	return (imaxX >= 0) && (iminX < scnSize.width) && (imaxY >= 0) && (iminY < scnSize.height);
}


Bool Map::ProjectedMapView::IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
{
	if (nPoints == 0)
	{
		return false;
	}
	Math::Coord2DDbl mul = this->scnSize.ToCoord() / (this->br - this->tl);
	Double dleft = this->tl.x;
	Double dbottom = this->br.y;
	Double rRate = 1 / mapRate;
	Int32 iminX = 0;
	Int32 iminY = 0;
	Int32 imaxX = 0;
	Int32 imaxY = 0;
	Int32 thisX;
	Int32 thisY;
	while (nPoints-- > 0)
	{
		*destArr++ = thisX = Double2Int32((*srcArr++ * rRate - dleft) * mul.x + ofstX);
		*destArr++ = thisY = Double2Int32((dbottom - *srcArr++ * rRate) * mul.y + ofstY);
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

Math::Coord2DDbl Map::ProjectedMapView::MapXYToScnXY(Math::Coord2DDbl mapPos)
{
	return Math::Coord2DDbl(mapPos.x - this->tl.x, this->br.y - mapPos.y) * scnSize.ToCoord() / (this->br - this->tl);
}

Math::Coord2DDbl Map::ProjectedMapView::ScnXYToMapXY(Math::Coord2DDbl scnPos)
{
	Math::Coord2DDbl v = scnPos * (this->br - this->tl) / this->scnSize.ToCoord();
	return Math::Coord2DDbl(this->tl.x + v.x, this->br.y - v.y);
}

Map::MapView *Map::ProjectedMapView::Clone()
{
	Map::ProjectedMapView *view;
	NEW_CLASS(view, Map::ProjectedMapView(this->scnSize, this->centMap, this->scale));
	return view;
}
