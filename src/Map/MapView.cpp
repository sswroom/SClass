#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapView.h"
#include "Math/Math.h"

Map::MapView::MapView(Math::Size2DDbl scnSize)
{
	this->scnSize = scnSize;
	this->converterOfst = Math::Coord2DDbl(0, 0);
	this->hAngle = 0;
	this->hSin = 0;
	this->hCos = 1;
	this->hISin = 0;
	this->hICos = 1;
}

Map::MapView::~MapView()
{
}

Math::Coord2DDbl Map::MapView::MapXYToScnXY(Math::Coord2DDbl mapPos) const
{
	Math::Coord2DDbl scnCenter = this->scnSize * 0.5;
	Math::Coord2DDbl diff = this->MapXYToScnXYNoDir(mapPos) - scnCenter;
	Double sAng = this->hISin;
	Double cAng = this->hICos;
	return scnCenter + Math::Coord2DDbl(diff.x * cAng + diff.y * sAng, -diff.x * sAng + diff.y * cAng);
}

Math::Coord2DDbl Map::MapView::ScnXYToMapXY(Math::Coord2DDbl scnPos) const
{
	Math::Coord2DDbl scnCenter = this->scnSize * 0.5;
	Math::Coord2DDbl diff = scnPos - scnCenter;
	Double sAng = this->hSin;
	Double cAng = this->hCos;
	return this->ScnXYNoDirToMapXY(scnCenter + Math::Coord2DDbl(diff.x * cAng + diff.y * sAng, -diff.x * sAng + diff.y * cAng));
}

Double Map::MapView::GetScnWidth() const
{
	return this->scnSize.GetWidth();
}

Double Map::MapView::GetScnHeight() const
{
	return this->scnSize.GetHeight();
}

Math::Size2DDbl Map::MapView::GetScnSize() const
{
	return this->scnSize;
}

void Map::MapView::SetVAngle(Double angleRad)
{
}

void Map::MapView::SetHAngle(Double angleRad)
{
	this->hAngle = angleRad;
	this->hSin = Math_Sin(angleRad);
	this->hCos = Math_Cos(angleRad);
	this->hISin = Math_Sin(-angleRad);
	this->hICos = Math_Cos(-angleRad);
}

void Map::MapView::SetDestImage(NN<Media::DrawImage> img)
{
	if (this->scnSize.GetWidth() != UOSInt2Double(img->GetWidth()) || this->scnSize.GetHeight() != UOSInt2Double(img->GetHeight()) || this->GetDDPI() != img->GetHDPI())
	{
		this->SetDPI(this->GetHDPI(), img->GetHDPI());
		ChangeViewXY(img->GetSize().ToDouble(), this->GetCenter(), this->GetMapScale());
	}
}

void Map::MapView::ToPointCnt(UnsafeArray<Int32> parts, Int32 nParts, Int32 nPoints) const
{
	Int32 tmpV;
	while (nParts-- > 0)
	{
		tmpV = parts[nParts];
		parts[nParts] = nPoints - tmpV;
		nPoints = tmpV;
	}
}

void Map::MapView::SetViewBounds(Math::RectAreaDbl bounds)
{
	Double currScale = this->GetMapScale();
	Math::RectAreaDbl rect = this->GetVerticalRect();
	Math::Coord2DDbl sz = bounds.GetSize();
	Double xRatio = rect.GetWidth() / sz.x;
	Double yRatio = rect.GetHeight() / sz.y;
	if (xRatio < 0)
		xRatio = -xRatio;
	if (yRatio < 0)
		yRatio = -yRatio;
	if (xRatio < yRatio)
	{
		ChangeViewXY(this->scnSize, bounds.GetCenter(), currScale / xRatio);
	}
	else if (yRatio == 0 || (currScale / yRatio < 400))
	{
		ChangeViewXY(this->scnSize, bounds.GetCenter(), 400);
	}
	else
	{
		ChangeViewXY(this->scnSize, bounds.GetCenter(), currScale / yRatio);
	}
}

void Map::MapView::SetConverterOfst(Math::Coord2DDbl ofst)
{
	this->converterOfst = ofst;
}

UInt32 Map::MapView::GetOutputSRID() const
{
	return SRID_SCREEN;
}

Math::Coord2DDbl Map::MapView::Convert2D(Math::Coord2DDbl coord) const
{
	return this->MapXYToScnXY(coord) + this->converterOfst;
}

Math::Vector3 Map::MapView::Convert3D(Math::Vector3 vec3) const
{
	return Math::Vector3(this->MapXYToScnXY(vec3.GetXY()) + this->converterOfst, 0);
}

void Map::MapView::Convert2DArr(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UOSInt nPoints) const
{
	this->MapXYToScnXYArr(srcArr, destArr, nPoints, this->converterOfst);
}
