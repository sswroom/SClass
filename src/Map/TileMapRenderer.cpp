#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Data/ArrayListInt64.h"
#include "Map/TileMapRenderer.h"

Map::TileMapRenderer::TileMapRenderer(NN<Media::DrawEngine> eng, Map::TileMap *map, NN<Parser::ParserList> parsers)
{
	this->eng = eng;
	this->map = map;
	this->parsers = parsers;
	this->lastLevel = -1;
}

Map::TileMapRenderer::~TileMapRenderer()
{
	CachedImage *cimg;
	OSInt i = this->lastImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->lastImgs.GetItem(i);
		this->eng->DeleteImage(cimg->img);
		MemFree(cimg);
	}
}

void Map::TileMapRenderer::DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OptOut<UInt32> imgDurMS)
{
	imgDurMS.Set(0);
	CachedImage *cimg;
	Data::ArrayListInt64 *cacheIds;
	Data::ArrayList<CachedImage *> *cacheImgs;

	Data::ArrayListInt64 idList;
	OSInt i;
	OSInt j;
	OSInt k;
	Media::ImageList *imgList;
	OSInt level = map->GetNearestLevel(view->GetMapScale());

	NEW_CLASS(cacheIds, Data::ArrayListInt64());
	NEW_CLASS(cacheImgs, Data::ArrayList<CachedImage*>());
	if (this->lastLevel == level)
	{
		cacheIds->AddAll(this->lastIds);
		cacheImgs->AddAll(this->lastImgs);
		this->lastIds.Clear();
		this->lastImgs.Clear();
	}
	else
	{
		j = this->lastImgs.GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs.GetItem(j);
			this->eng->DeleteImage(cimg->img);
			MemFree(cimg);
		}
		this->lastIds.Clear();
		this->lastImgs.Clear();
	}

	Double bounds[4];
	Double lat = view->GetTopLat();
	Double lon = view->GetLeftLon();
	Double lat2 = view->GetBottomLat();
	Double lon2 = view->GetRightLon();
	Double scnX;
	Double scnY;
	map->GetImageIDs(level, lon, lat, lon2, lat2, idList);
	i = idList.GetCount();
	while (i-- > 0)
	{
		j = cacheIds->SortedIndexOf(idList.GetItem(i));
		if (j >= 0)
		{
			k = this->lastIds.SortedInsert(cacheIds->RemoveAt(j));
			this->lastImgs.Insert(k, cimg = cacheImgs->RemoveAt(j));

			view->LatLonToScnXY(cimg->tly, cimg->tlx, &scnX, &scnY);
			img->DrawImagePt(cimg->img, Math::Coord2DDbl(scnX, scnY));
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
				img->DrawImagePt(cimg->img, Math::Coord2DDbl(scnX, scnY));
				DEL_CLASS(imgList);

				k = this->lastIds.SortedInsert(idList.GetItem(i));
				this->lastImgs.Insert(k, cimg);
			}
		}
	}

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

void Map::TileMapRenderer::SetUpdatedHandler(UpdatedHandler updHdlr, AnyType userObj)
{
}
