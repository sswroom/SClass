#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/TileMapScheduler.h"

void Map::TileMapScheduler::DrawPoints(Map::DrawObject *dobj)
{
	OSInt j;
	Int32 *objPtr = &this->objBounds[4 * *this->objCnt];
	Int32 pts[2];
	Int32 imgW;
	Int32 imgH;
	Int32 scnW = this->map->GetScnWidth();
	Int32 scnH = this->map->GetScnHeight();
	imgW = this->ico->GetWidth() >> 1;
	imgH = this->ico->GetHeight() >> 1;
	j = dobj->nPoints;
	while (j-- > 0)
	{
		if (this->map->MapToScnXY(&dobj->points[j << 1], pts, 1, 0, 0))
			*this->isLayerEmpty = false;
		objPtr[0] = pts[0] - imgW;
		objPtr[1] = pts[1] - imgH;
		objPtr[2] = pts[0] + imgW;
		objPtr[3] = pts[1] + imgH;
		if (objPtr[0] < scnW * 2 && objPtr[1] < scnH * 2 && objPtr[2] >= -scnW && objPtr[3] >= -scnH)
		{
			this->img->DrawImagePt(this->ico, objPtr[0], objPtr[1]);
			objPtr += 4;
			++*(this->objCnt);
		}
	}
}

Map::TileMapScheduler::TileMapScheduler() : Map::MapScheduler()
{
}

Map::TileMapScheduler::~TileMapScheduler()
{
}
