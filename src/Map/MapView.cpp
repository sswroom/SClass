#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapView.h"
#include "Math/Math.h"

Map::MapView::MapView(UOSInt scnWidth, UOSInt scnHeight)
{
	this->scnWidth = scnWidth;
	this->scnHeight = scnHeight;
}

Map::MapView::~MapView()
{
}

UOSInt Map::MapView::GetScnWidth()
{
	return this->scnWidth;
}

UOSInt Map::MapView::GetScnHeight()
{
	return this->scnHeight;
}

void Map::MapView::SetDestImage(Media::DrawImage *img)
{
	if (this->scnWidth != img->GetWidth() || this->scnHeight != img->GetHeight() || this->GetDDPI() != img->GetHDPI())
	{
		this->SetDPI(this->GetHDPI(), img->GetHDPI());
		ChangeViewXY(img->GetWidth(), img->GetHeight(), this->GetCenterX(), this->GetCenterY(), this->GetMapScale());
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
		ChangeViewXY(this->scnWidth, this->scnHeight, (x1 + x2) * 0.5, (y1 + y2) * 0.5, currScale / xRatio);
	}
	else
	{
		ChangeViewXY(this->scnWidth, this->scnHeight, (x1 + x2) * 0.5, (y1 + y2) * 0.5, currScale / yRatio);
	}
}
