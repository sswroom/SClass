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

Double Map::MapView::GetScnWidth()
{
	return this->scnSize.width;
}

Double Map::MapView::GetScnHeight()
{
	return this->scnSize.height;
}

Math::Size2D<Double> Map::MapView::GetScnSize()
{
	return this->scnSize;
}

void Map::MapView::SetDestImage(Media::DrawImage *img)
{
	if (this->scnSize.width != UOSInt2Double(img->GetWidth()) || this->scnSize.height != UOSInt2Double(img->GetHeight()) || this->GetDDPI() != img->GetHDPI())
	{
		this->SetDPI(this->GetHDPI(), img->GetHDPI());
		ChangeViewXY(Math::Size2D<Double>(UOSInt2Double(img->GetWidth()), UOSInt2Double(img->GetHeight())), this->GetCenter(), this->GetMapScale());
	}
}

void Map::MapView::ToPointCnt(Int32 *parts, Int32 nParts, Int32 nPoints)
{
	Int32 tmpV;
	while (nParts-- > 0)
	{
		tmpV = parts[nParts];
		parts[nParts] = nPoints - tmpV;
		nPoints = tmpV;
	}
}

void Map::MapView::SetViewBounds(Double x1, Double y1, Double x2, Double y2)
{
	Double currScale = this->GetMapScale();
	Double left = this->GetLeftX();
	Double right = this->GetRightX();
	Double top = this->GetTopY();
	Double bottom = this->GetBottomY();
	Double xRatio = (right - left) / (x2 - x1);
	Double yRatio = (bottom - top) / (y2 - y1);
	if (xRatio < 0)
		xRatio = -xRatio;
	if (yRatio < 0)
		yRatio = -yRatio;
	if (xRatio < yRatio)
	{
		ChangeViewXY(this->scnSize, Math::Coord2DDbl((x1 + x2) * 0.5, (y1 + y2) * 0.5), currScale / xRatio);
	}
	else
	{
		ChangeViewXY(this->scnSize, Math::Coord2DDbl((x1 + x2) * 0.5, (y1 + y2) * 0.5), currScale / yRatio);
	}
}
