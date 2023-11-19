#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/LeveledMapView.h"
#include "Math/Math.h"

void Map::LeveledMapView::UpdateVals()
{
	Double scale = this->scales.GetItem(this->level);
	Math::Coord2DDbl diff;
	if (this->projected)
	{
		diff = this->scnSize * (0.5 * scale / (this->hdpi * 72.0 / 96) * 0.0254);
	}
	else
	{
		diff = this->scnSize * (0.00025 * scale / (this->hdpi * 72.0 / 96) * 0.000254);
	}

	this->tl = this->centMap - diff;
	this->br = this->centMap + diff;
}

Map::LeveledMapView::LeveledMapView(Bool projected, Math::Size2DDbl scnSize, Math::Coord2DDbl center, NotNullPtr<const Data::ArrayListDbl> scales) : Map::MapView(scnSize)
{
	this->projected = projected;
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	this->scales.AddAll(scales);
	ChangeViewXY(scnSize, center, Double2Int32(this->scales.GetItem(this->scales.GetCount() >> 1)));
}

Map::LeveledMapView::~LeveledMapView()
{
}

void Map::LeveledMapView::ChangeViewXY(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale)
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
	i = this->scales.GetCount();
	while (i-- > 0)
	{
		ldiff = Math_Log10(this->scales.GetItem(i)) - logResol;
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

void Map::LeveledMapView::UpdateSize(Math::Size2DDbl scnSize)
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

Math::Quadrilateral Map::LeveledMapView::GetBounds() const
{
	return Math::Quadrilateral(this->tl, Math::Coord2DDbl(this->br.x, this->tl.y), this->br, Math::Coord2DDbl(this->tl.x, this->br.y));
}

Math::RectAreaDbl Map::LeveledMapView::GetVerticalRect() const
{
	Math::Coord2DDbl sz = this->br - this->tl;
	return Math::RectAreaDbl(this->tl.x, this->tl.y, sz.x, sz.y);
}

Double Map::LeveledMapView::GetMapScale() const
{
	return this->scales.GetItem(this->level);
}

Double Map::LeveledMapView::GetViewScale() const
{
	return this->scales.GetItem(this->level);
}

Math::Coord2DDbl Map::LeveledMapView::GetCenter() const
{
	return this->centMap;
}

Double Map::LeveledMapView::GetHDPI() const
{
	return this->hdpi;
}

Double Map::LeveledMapView::GetDDPI() const
{
	return this->ddpi;
}

Bool Map::LeveledMapView::InViewXY(Math::Coord2DDbl mapPos) const
{
	return mapPos >= this->tl && mapPos < this->br;
}

Bool Map::LeveledMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const
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
		destArr[0].x = thisX = Double2Int32((srcArr->x  - dleft) * mul.x + ofst.x);
		destArr[0].y = thisY = Double2Int32((dbottom - srcArr->y) * mul.y + ofst.y);
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

Bool Map::LeveledMapView::MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst) const
{
	if (nPoints == 0)
	{
		return false;
	}

	Math::Coord2DDbl mul = this->scnSize / (this->br - this->tl);
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
	return (imaxX >= 0) && (iminX < scnSize.x) && (imaxY >= 0) && (iminY < scnSize.y);
}

Bool Map::LeveledMapView::IMapXYToScnXY(Double mapRate, const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const
{
	if (nPoints == 0)
	{
		return false;
	}
	Math::Coord2DDbl mul = this->scnSize / (this->br - this->tl);
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
		destArr[0].x = thisX = Double2Int32((srcArr[0].x * rRate - dleft) * mul.x + ofst.x);
		destArr[0].y = thisY = Double2Int32((dbottom - srcArr[0].y * rRate) * mul.y + ofst.y);
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

Math::Coord2DDbl Map::LeveledMapView::MapXYToScnXY(Math::Coord2DDbl mapPos) const
{
	return Math::Coord2DDbl(mapPos.x - this->tl.x, this->br.y - mapPos.y) * scnSize / (this->br - this->tl);
}

Math::Coord2DDbl Map::LeveledMapView::ScnXYToMapXY(Math::Coord2DDbl scnPos) const
{
	Math::Coord2DDbl v = scnPos * (this->br - this->tl) / scnSize;
	return Math::Coord2DDbl(this->tl.x + v.x, this->br.y - v.y);
}

NotNullPtr<Map::MapView> Map::LeveledMapView::Clone() const
{
	NotNullPtr<Map::LeveledMapView> view;
	NEW_CLASSNN(view, Map::LeveledMapView(this->projected, this->scnSize, this->centMap, this->scales));
	return view;
}
