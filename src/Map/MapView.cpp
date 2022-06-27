#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapView.h"
#include "Math/Math.h"

Map::MapView::MapView(Math::Size2D<Double> scnSize)
{
	this->scnSize = scnSize;
}

Map::MapView::~MapView()
{
}

Double Map::MapView::GetScnWidth() const
{
	return this->scnSize.width;
}

Double Map::MapView::GetScnHeight() const
{
	return this->scnSize.height;
}

Math::Size2D<Double> Map::MapView::GetScnSize() const
{
	return this->scnSize;
}

void Map::MapView::SetVAngle(Double angleRad)
{
}

void Map::MapView::SetDestImage(Media::DrawImage *img)
{
	if (this->scnSize.width != UOSInt2Double(img->GetWidth()) || this->scnSize.height != UOSInt2Double(img->GetHeight()) || this->GetDDPI() != img->GetHDPI())
	{
		this->SetDPI(this->GetHDPI(), img->GetHDPI());
		ChangeViewXY(Math::Size2D<Double>(UOSInt2Double(img->GetWidth()), UOSInt2Double(img->GetHeight())), this->GetCenter(), this->GetMapScale());
	}
}

void Map::MapView::ToPointCnt(Int32 *parts, Int32 nParts, Int32 nPoints) const
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
	else
	{
		ChangeViewXY(this->scnSize, bounds.GetCenter(), currScale / yRatio);
	}
}
