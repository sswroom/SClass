#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Data/ArrayListInt64.h"
#include "Map/TileMapRenderer.h"

Map::TileMapRenderer::TileMapRenderer(NotNullPtr<Media::DrawEngine> eng, Map::TileMap *map, Parser::ParserList *parsers)
{
	this->eng = eng;
	this->map = map;
	this->parsers = parsers;
	this->lastLevel = -1;
	NEW_CLASS(this->lastIds, Data::ArrayListInt64());
	NEW_CLASS(this->lastImgs, Data::ArrayList<CachedImage*>());

}

Map::TileMapRenderer::~TileMapRenderer()
{
	CachedImage *cimg;
	OSInt i = this->lastImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->lastImgs->GetItem(i);
		this->eng->DeleteImage(cimg->img);
		MemFree(cimg);
	}
	DEL_CLASS(this->lastImgs);
	DEL_CLASS(this->lastIds);
}

void Map::TileMapRenderer::DrawMap(Media::DrawImage *img, Map::MapView *view)
{
	CachedImage *cimg;
	Data::ArrayListInt64 *cacheIds;
	Data::ArrayList<CachedImage *> *cacheImgs;

	Data::ArrayListInt64 *idList;
	OSInt i;
	OSInt j;
	OSInt k;
	Media::ImageList *imgList;
	NEW_CLASS(idList, Data::ArrayListInt64());
	OSInt level = map->GetNearestLevel(view->GetScale());

	NEW_CLASS(cacheIds, Data::ArrayListInt64());
	NEW_CLASS(cacheImgs, Data::ArrayList<CachedImage*>());
	if (this->lastLevel == level)
	{
		cacheIds->AddRange(this->lastIds);
		cacheImgs->AddRange(this->lastImgs);
		this->lastIds->Clear();
		this->lastImgs->Clear();
	}
	else
	{
		j = this->lastImgs->GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs->GetItem(j);
			this->eng->DeleteImage(cimg->img);
			MemFree(cimg);
		}
		this->lastIds->Clear();
		this->lastImgs->Clear();
	}

	Double bounds[4];
	Double lat = view->GetTopLat();
	Double lon = view->GetLeftLon();
	Double lat2 = view->GetBottomLat();
	Double lon2 = view->GetRightLon();
	Double scnX;
	Double scnY;
	map->GetImageIDs(level, lon, lat, lon2, lat2, idList);
	i = idList->GetCount();
	while (i-- > 0)
	{
		j = cacheIds->SortedIndexOf(idList->GetItem(i));
		if (j >= 0)
		{
			k = this->lastIds->SortedInsert(cacheIds->RemoveAt(j));
			this->lastImgs->Insert(k, cimg = cacheImgs->RemoveAt(j));

			view->LatLonToScnXY(cimg->tly, cimg->tlx, &scnX, &scnY);
			img->DrawImagePt(cimg->img, Math::Double2Int(scnX), Math::Double2Int(scnY));
		}
		else
		{
			imgList = this->map->LoadTileImage(level, idList->GetItem(i), this->parsers, bounds, false);
			if (imgList)
			{
				cimg = MemAlloc(CachedImage, 1);
				cimg->tlx = bounds[0];
				cimg->tly = bounds[1];
				cimg->img = this->eng->ConvImage(imgList->GetImage(0, 0));
				view->LatLonToScnXY(cimg->tly, cimg->tlx, &scnX, &scnY);
				img->DrawImagePt(cimg->img, Math::Double2Int(scnX), Math::Double2Int(scnY));
				DEL_CLASS(imgList);

				k = this->lastIds->SortedInsert(idList->GetItem(i));
				this->lastImgs->Insert(k, cimg);
			}
		}
	}
	DEL_CLASS(idList);

	lastLevel = level;
	i = cacheImgs->GetCount();
	while (i-- > 0)
	{
		cimg = cacheImgs->GetItem(i);
		this->eng->DeleteImage(cimg->img);
		MemFree(cimg);
	}
	DEL_CLASS(cacheIds);
	DEL_CLASS(cacheImgs);
}

void Map::TileMapRenderer::SetUpdatedHandler(UpdatedHandler updHdlr, void *userObj)
{
}
