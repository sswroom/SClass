#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/LeveledMapView.h"
#include "Math/Math.h"

void Map::LeveledMapView::UpdateVals()
{
	Double scale = this->scales->GetItem(this->level);
	Math::Coord2DDbl diff = this->scnSize.ToCoord() * (0.00025 * scale / (this->hdpi * 72.0 / 96) * 2.54 / 10000.0);

	this->tl = this->centMap - diff;
	this->br = this->centMap + diff;
}

Map::LeveledMapView::LeveledMapView(Math::Size2D<Double> scnSize, Double centLat, Double centLon, Data::ArrayListDbl *scales) : Map::MapView(scnSize)
{
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	NEW_CLASS(this->scales, Data::ArrayListDbl());
	this->scales->AddAll(scales);
	ChangeViewXY(scnSize, Math::Coord2DDbl(centLon, centLat), Double2Int32(this->scales->GetItem(this->scales->GetCount() >> 1)));
}

Map::LeveledMapView::~LeveledMapView()
{
	DEL_CLASS(this->scales);
}

void Map::LeveledMapView::ChangeViewXY(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale)
{
	this->centMap = centMap;
	this->scnSize = scnSize;
	this->SetMapScale(scale);
}

void Map::LeveledMapView::SetCenterXY(Math::Coord2DDbl mapPos)
{
	this->centMap = mapPos;
	this->UpdateVals();
}

void Map::LeveledMapView::SetMapScale(Double scale)
{
	Double ldiff;
	Double minDiff;
	UOSInt minInd;
	UOSInt i;
	Double logResol = Math_Log10(scale);
	minInd = 0;
	minDiff = 100000.0;
	i = this->scales->GetCount();
	while (i-- > 0)
	{
		ldiff = Math_Log10(this->scales->GetItem(i)) - logResol;
		if (ldiff < 0)
			ldiff = -ldiff;
		if (ldiff < minDiff)
		{
			minDiff = ldiff;
			minInd = i;
		}
	}
	this->level = (UInt32)minInd;
	this->UpdateVals();
}

void Map::LeveledMapView::UpdateSize(Math::Size2D<Double> scnSize)
{
	this->scnSize = scnSize;
	this->UpdateVals();
}

void Map::LeveledMapView::SetDPI(Double hdpi, Double ddpi)
{
	if (hdpi > 0 && ddpi > 0 && (this->hdpi != hdpi || this->ddpi != ddpi))
	{
		this->hdpi = hdpi;
		this->ddpi = ddpi;
		this->UpdateVals();
	}
}

Double Map::LeveledMapView::GetLeftX()
{
	return this->tl.x;
}

Double Map::LeveledMapView::GetTopY()
{
	return this->tl.y;
}

Double Map::LeveledMapView::GetRightX()
{
	return this->br.x;
}

Double Map::LeveledMapView::GetBottomY()
{
	return this->br.y;
}

Double Map::LeveledMapView::GetMapScale()
{
	return this->scales->GetItem(this->level);
}

Double Map::LeveledMapView::GetViewScale()
{
	return this->scales->GetItem(this->level);
}

Math::Coord2DDbl Map::LeveledMapView::GetCenter()
{
	return this->centMap;
}

Double Map::LeveledMapView::GetHDPI()
{
	return this->hdpi;
}

Double Map::LeveledMapView::GetDDPI()
{
	return this->ddpi;
}

Bool Map::LeveledMapView::InViewXY(Math::Coord2DDbl mapPos)
{
	return mapPos >= this->tl && mapPos < this->br;
}

Bool Map::LeveledMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
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

Bool Map::LeveledMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst)
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
		destArr[0] = thisPt = Math::Coord2DDbl(srcArr->x  - dleft, dbottom - srcArr->y) * mul + ofst;
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

Bool Map::LeveledMapView::IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY)
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

Math::Coord2DDbl Map::LeveledMapView::MapXYToScnXY(Math::Coord2DDbl mapPos)
{
	return Math::Coord2DDbl(mapPos.x - this->tl.x, this->br.y - mapPos.y) * scnSize.ToCoord() / (this->br - this->tl);
}

Math::Coord2DDbl Map::LeveledMapView::ScnXYToMapXY(Math::Coord2DDbl scnPos)
{
	Math::Coord2DDbl v = scnPos * (this->br - this->tl) / scnSize.ToCoord();
	return Math::Coord2DDbl(this->tl.x + v.x, this->br.y - v.y);
}

Map::MapView *Map::LeveledMapView::Clone()
{
	Map::LeveledMapView *view;
	NEW_CLASS(view, Map::LeveledMapView(this->scnSize, this->centMap.y, this->centMap.x, this->scales));
	return view;
}
