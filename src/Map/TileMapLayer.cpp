#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/LeveledMapView.h"
#include "Map/MercatorMapView.h"
#include "Map/ScaledMapView.h"
#include "Map/TileMapLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/VectorImage.h"
#include "Media/ImageList.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

UInt32 __stdcall Map::TileMapLayer::TaskThread(void *userObj)
{
	Double bounds[4];
	Media::ImageList *imgList;
	CachedImage *cimg;
	ThreadStat *stat = (ThreadStat*)userObj;
	stat->running = true;
	stat->isIdle = false;
	while (!stat->toStop)
	{
		while (!stat->toStop)
		{
			Sync::MutexUsage mutUsage(stat->me->taskMut);
			cimg = (CachedImage*)stat->me->taskQueued->Get();
			mutUsage.EndUse();
			if (cimg == 0)
				break;

			if (cimg->isCancel)
			{
				cimg->isFinish = true;
			}
			else
			{
				imgList = stat->me->tileMap->LoadTileImage(cimg->level, cimg->imgId, stat->me->parsers, bounds, false);
				if (imgList)
				{
					NEW_CLASS(cimg->img, Media::SharedImage(imgList, false));
					cimg->isFinish = true;
					
					Sync::MutexUsage mutUsage(stat->me->updMut);
					UOSInt i = stat->me->updHdlrs->GetCount();
					while (i-- > 0)
					{
						stat->me->updHdlrs->GetItem(i)(stat->me->updObjs->GetItem(i));
					}
					mutUsage.EndUse();
				}
				else
				{
					cimg->isFinish = true;
				}
			}
		}
		stat->isIdle = true;
		stat->evt->Wait(1000);
		stat->isIdle = false;
	}
	stat->running = false;
	return 0;
}

void Map::TileMapLayer::AddTask(CachedImage *cimg)
{
	Sync::MutexUsage mutUsage(this->taskMut);
	this->taskQueued->Put(cimg);
	mutUsage.EndUse();
	this->threads[this->threadNext].evt->Set();
	this->threadNext = (this->threadNext + 1) % this->threadCnt;
}

void Map::TileMapLayer::CheckCache(Data::ArrayListInt64 *currIDs)
{
	if (this->lastIds->GetCount() <= 0)
		return;

	CachedImage *cimg;
	Data::ArrayListInt64 *cacheIds;
	Data::ArrayList<CachedImage *> *cacheImgs;
	UOSInt i;
	OSInt j;
	UOSInt k;
	NEW_CLASS(cacheIds, Data::ArrayListInt64());
	NEW_CLASS(cacheImgs, Data::ArrayList<CachedImage*>());
	Sync::MutexUsage lastMutUsage(this->lastMut);
	cacheIds->AddAll(this->lastIds);
	cacheImgs->AddAll(this->lastImgs);
	this->lastIds->Clear();
	this->lastImgs->Clear();

	i = currIDs->GetCount();
	while (i-- > 0)
	{
		j = cacheIds->SortedIndexOf(currIDs->GetItem(i));
		if (j >= 0)
		{
			k = this->lastIds->SortedInsert(cacheIds->RemoveAt((UOSInt)j));
			this->lastImgs->Insert(k, cimg = cacheImgs->RemoveAt((UOSInt)j));
		}
	}
	lastMutUsage.EndUse();

	Sync::MutexUsage idleMutUsage(this->idleMut);
	i = cacheImgs->GetCount();
	while (i-- > 0)
	{
		cimg = cacheImgs->GetItem(i);
		if (cimg->isFinish)
		{
			if (cimg->img)
			{
				DEL_CLASS(cimg->img);
			}
			MemFree(cimg);
		}
		else
		{
			cimg->isCancel = true;
			this->idleImgs->Add(cimg);
		}
	}
	idleMutUsage.EndUse();
	DEL_CLASS(cacheIds);
	DEL_CLASS(cacheImgs);

	idleMutUsage.BeginUse();
	i = this->idleImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs->GetItem(i);
		if (cimg->isFinish)
		{
			this->idleImgs->RemoveAt(i);
			if (cimg->img)
			{
				DEL_CLASS(cimg->img);
			}
			MemFree(cimg);
		}
	}
	idleMutUsage.EndUse();
}

Map::TileMapLayer::TileMapLayer(Map::TileMap *tileMap, Parser::ParserList *parsers) : Map::IMapDrawLayer(tileMap->GetName(), 0, 0)
{
	this->parsers = parsers;
	this->tileMap = tileMap;
	this->scale = 10000;

	this->lastLevel = (UOSInt)-1;
	this->threadNext = 0;
	NEW_CLASS(this->updMut, Sync::Mutex());
	NEW_CLASS(this->updHdlrs, Data::ArrayList<UpdatedHandler>());
	NEW_CLASS(this->updObjs, Data::ArrayList<void *>());
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
	NEW_CLASS(this->lastIds, Data::ArrayListInt64());
	NEW_CLASS(this->lastImgs, Data::ArrayList<CachedImage*>());
	NEW_CLASS(this->lastMut, Sync::Mutex());

	NEW_CLASS(this->idleImgs, Data::ArrayList<CachedImage*>());
	NEW_CLASS(this->idleMut, Sync::Mutex());
	NEW_CLASS(this->taskQueued, Data::LinkedList());
	NEW_CLASS(this->taskEvt, Sync::Event(true, (const UTF8Char*)"Map.ResizableTileMapRenderer.taskEvt"));
	NEW_CLASS(this->taskMut, Sync::Mutex());

	UOSInt i;
	this->threadCnt = this->tileMap->GetConcurrentCount();
	if (this->threadCnt <= 0)
		this->threadCnt = 1;
	this->threads = MemAlloc(ThreadStat, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].running = false;
		this->threads[i].toStop = false;
		this->threads[i].me = this;
		this->threads[i].isIdle = false;
		NEW_CLASS(this->threads[i].evt, Sync::Event(true, (const UTF8Char*)"Map.ResizableTileMapRenderer.threads.evt"));
		Sync::Thread::Create(TaskThread, &this->threads[i]);
	}
}

Map::TileMapLayer::~TileMapLayer()
{
	CachedImage *cimg;
	UOSInt i;
	Bool running;
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs->Clear();
	this->updObjs->Clear();
	mutUsage.EndUse();

	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].toStop = true;
		this->threads[i].evt->Set();
	}
	while (true)
	{
		running = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].running)
			{
				running = true;
				this->threads[i].evt->Set();
				break;
			}
		}
		if (!running)
			break;
	}

	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threads[i].evt);
	}
	MemFree(this->threads);

	i = this->lastImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->lastImgs->GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFree(cimg);
	}
	i = this->idleImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs->GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFree(cimg);
	}
	DEL_CLASS(this->taskQueued);
	DEL_CLASS(this->taskEvt);
	DEL_CLASS(this->taskMut);
	DEL_CLASS(this->idleImgs);
	DEL_CLASS(this->idleMut);
	DEL_CLASS(this->lastImgs);
	DEL_CLASS(this->lastIds);
	DEL_CLASS(this->lastMut);
	DEL_CLASS(this->updMut);
	DEL_CLASS(this->updHdlrs);
	DEL_CLASS(this->updObjs);
	DEL_CLASS(this->tileMap);
}

void Map::TileMapLayer::SetCurrScale(Double scale)
{
	CachedImage *cimg;
	UOSInt j;

	this->scale = scale;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (this->lastLevel != level)
	{
		Sync::MutexUsage lastMutUsage(this->lastMut);
		Sync::MutexUsage idleMutUsage(this->idleMut);
		this->lastLevel = level;
		j = this->lastImgs->GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs->GetItem(j);
			if (cimg->isFinish)
			{
				if (cimg->img)
				{
					DEL_CLASS(cimg->img);
				}
				MemFree(cimg);
			}
			else
			{
				cimg->isCancel = true;
				this->idleImgs->Add(cimg);
			}
		}
		this->lastIds->Clear();
		this->lastImgs->Clear();
		idleMutUsage.EndUse();
		lastMutUsage.EndUse();
	}
}

Map::MapView *Map::TileMapLayer::CreateMapView(UOSInt width, UOSInt height)
{
	Map::MapView *view;
	if (this->tileMap->GetProjectionType() == Map::TileMap::PT_MERCATOR)
	{
		NEW_CLASS(view, Map::MercatorMapView(width, height, 22.4, 114.2, this->tileMap->GetLevelCount(), this->tileMap->GetTileSize()));
		return view;
	}
	else if (this->tileMap->GetProjectionType() == Map::TileMap::PT_WGS84)
	{
		Data::ArrayListDbl scales;
		UOSInt i = 0;
		UOSInt j = this->tileMap->GetLevelCount();
		while (i < j)
		{
			scales.Add(this->tileMap->GetLevelScale(i));
			i++;
		}
		NEW_CLASS(view, Map::LeveledMapView(width, height, 22.4, 114.2, &scales));
		return view;
	}
	else
	{
		NEW_CLASS(view, Map::ScaledMapView(width, height, 22.4, 114.2, 10000));
		return view;
	}
}

Map::DrawLayerType Map::TileMapLayer::GetLayerType()
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::TileMapLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt retCnt;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (nameArr)
		*nameArr = 0;
	retCnt = this->tileMap->GetImageIDs(level, -180, -90, 180, 90, outArr);
	return retCnt;
}

UOSInt Map::TileMapLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	UOSInt retCnt;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (nameArr)
		*nameArr = 0;
	retCnt = this->tileMap->GetImageIDs(level, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, outArr);
	CheckCache(outArr);
	return retCnt;
}

UOSInt Map::TileMapLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	UOSInt retCnt;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (nameArr)
		*nameArr = 0;
	retCnt = this->tileMap->GetImageIDs(level, x1, y1, x2, y2, outArr);
	CheckCache(outArr);
	return retCnt;
}

Int64 Map::TileMapLayer::GetObjectIdMax()
{
	return 0;
}

void Map::TileMapLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::TileMapLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	switch (strIndex)
	{
	case 0:
		return Text::StrInt64(buff, id);
	case 1:
		return Text::StrUOSInt(buff, this->tileMap->GetNearestLevel(scale));
	case 2:
		return this->tileMap->GetImageURL(buff, this->tileMap->GetNearestLevel(scale), id);
	}
	return 0;
}

UOSInt Map::TileMapLayer::GetColumnCnt()
{
	return 3;
}

UTF8Char *Map::TileMapLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	switch (colIndex)
	{
	case 0:
		return Text::StrConcat(buff, (const UTF8Char*)"id");
	case 1:
		return Text::StrConcat(buff, (const UTF8Char*)"level");
	case 2:
		return Text::StrConcat(buff, (const UTF8Char*)"url");
	}
	return 0;
}

DB::DBUtil::ColType Map::TileMapLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	switch (colIndex)
	{
	case 0:
		if (colSize) *colSize = 20;
		return DB::DBUtil::CT_Int64;
	case 1:
		if (colSize) *colSize = 10;
		return DB::DBUtil::CT_UInt32;
	case 2:
		if (colSize) *colSize = 512;
		return DB::DBUtil::CT_VarChar;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::TileMapLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	UTF8Char sbuff[256];
	UOSInt colSize;
	switch (colIndex)
	{
	case 0:
		colDef->SetPK(true);
	case 1:
	case 2:
		this->GetColumnName(sbuff, colIndex);
		colDef->SetColName(sbuff);
		colDef->SetColType(this->GetColumnType(colIndex, &colSize));
		colDef->SetColSize(colSize);
		colDef->SetColDP(0);
		colDef->SetNotNull(true);
		colDef->SetAutoInc(false);
		return true;
	}
	return false;
}

UInt32 Map::TileMapLayer::GetCodePage()
{
	return 0;
}

Bool Map::TileMapLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	return this->tileMap->GetBounds(minX, minY, maxX, maxY);
}

void *Map::TileMapLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::TileMapLayer::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::TileMapLayer::GetObjectByIdD(void *session, Int64 id)
{
	return 0;
}

Math::Vector2D *Map::TileMapLayer::GetVectorById(void *session, Int64 id)
{
	CachedImage *cimg;
	Math::VectorImage *vimg;
	OSInt i;
	UOSInt k;
	Media::ImageList *imgList;
	Double bounds[4];
	UTF8Char u8buff[512];
	UOSInt level = this->tileMap->GetNearestLevel(scale);

	i = this->lastIds->SortedIndexOf(id);
	if (i >= 0)
	{
		cimg = this->lastImgs->GetItem((UOSInt)i);
		if (cimg->img == 0)
			return 0;
		this->tileMap->GetImageURL(u8buff, cimg->level, cimg->imgId);
		NEW_CLASS(vimg, Math::VectorImage(this->csys->GetSRID(), cimg->img, cimg->tlx, cimg->tly, cimg->brx, cimg->bry, false, u8buff, 0, 0));
		return vimg;
	}

	if (this->IsCaching(level, id))
	{
		return 0;
	}
	imgList = this->tileMap->LoadTileImage(level, id, this->parsers, bounds, true);
	if (imgList)
	{
		cimg = MemAlloc(CachedImage, 1);
		cimg->imgId = id;
		cimg->tlx = bounds[0];
		cimg->tly = bounds[1];
		cimg->brx = bounds[2];
		cimg->bry = bounds[3];
		cimg->level = level;
		cimg->isFinish = true;
		cimg->isCancel = false;
		NEW_CLASS(cimg->img, Media::SharedImage(imgList, false));

		Sync::MutexUsage mutUsage(this->lastMut);
		k = this->lastIds->SortedInsert(id);
		this->lastImgs->Insert(k, cimg);
		mutUsage.EndUse();

		this->tileMap->GetImageURL(u8buff, level, id);
		NEW_CLASS(vimg, Math::VectorImage(this->csys->GetSRID(), cimg->img, cimg->tlx, cimg->tly, cimg->brx, cimg->bry, false, u8buff, 0, 0));
		return vimg;
	}
	else
	{
		cimg = MemAlloc(CachedImage, 1);
		cimg->imgId = id;
		cimg->tlx = bounds[0];
		cimg->tly = bounds[1];
		cimg->brx = bounds[2];
		cimg->bry = bounds[3];
		cimg->level = level;
		cimg->isFinish = false;
		cimg->isCancel = false;
		cimg->img = 0;
		AddTask(cimg);

		Sync::MutexUsage mutUsage(this->lastMut);
		k = this->lastIds->SortedInsert(id);
		this->lastImgs->Insert(k, cimg);
		mutUsage.EndUse();

		return 0;
	}
}

void Map::TileMapLayer::ReleaseObject(void *session, DrawObjectL *obj)
{
}

Map::IMapDrawLayer::ObjectClass Map::TileMapLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_TILE_MAP_LAYER;
}

void Map::TileMapLayer::AddUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs->Add(hdlr);
	this->updObjs->Add(obj);
	mutUsage.EndUse();
}

void Map::TileMapLayer::RemoveUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs->GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs->GetItem(i) == hdlr && this->updObjs->GetItem(i) == obj)
		{
			this->updObjs->RemoveAt(i);
			this->updHdlrs->RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

Bool Map::TileMapLayer::IsCaching(UOSInt level, Int64 imgId)
{
	UOSInt i;
	Bool found = false;
	CachedImage *cimg;
	Sync::MutexUsage mutUsage(this->idleMut);
	i = this->idleImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs->GetItem(i);
		if (cimg->level == level && cimg->imgId == imgId && !cimg->isFinish)
		{
			found = true;
			break;
		}
	}
	mutUsage.EndUse();
	return found;
}

void Map::TileMapLayer::WaitCache()
{
	Bool found;
	UOSInt i;
	while (true)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (!this->threads[i].isIdle)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		Sync::Thread::Sleep(10);
	}
}

Map::TileMap *Map::TileMapLayer::GetTileMap()
{
	return this->tileMap;
}
