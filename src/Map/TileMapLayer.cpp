#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/LeveledMapView.h"
#include "Map/MercatorMapView.h"
#include "Map/ScaledMapView.h"
#include "Map/TileMapLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImageList.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Map::TileMapLayer::TaskThread(void *userObj)
{
	Math::RectAreaDbl bounds;
	Media::ImageList *imgList;
	CachedImage *cimg;
	ThreadStat *stat = (ThreadStat*)userObj;
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->running = true;
		stat->isIdle = false;
		while (!stat->toStop)
		{
			while (!stat->toStop)
			{
				Sync::MutexUsage mutUsage(&stat->me->taskMut);
				cimg = (CachedImage*)stat->me->taskQueued.Get();
				mutUsage.EndUse();
				if (cimg == 0)
					break;

				if (cimg->isCancel)
				{
					cimg->isFinish = true;
				}
				else
				{
					imgList = stat->me->tileMap->LoadTileImage(cimg->level, IdToCoord(cimg->imgId), stat->me->parsers, &bounds, false);
					if (imgList)
					{
						NEW_CLASS(cimg->img, Media::SharedImage(imgList, false));
						cimg->isFinish = true;
						
						Sync::MutexUsage mutUsage(&stat->me->updMut);
						UOSInt i = stat->me->updHdlrs.GetCount();
						while (i-- > 0)
						{
							stat->me->updHdlrs.GetItem(i)(stat->me->updObjs.GetItem(i));
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
	}
	stat->running = false;
	return 0;
}

Math::Coord2D<Int32> Map::TileMapLayer::IdToCoord(Int64 id)
{
	return Math::Coord2D<Int32>((Int32)(id >> 32),
		(Int32)(id & 0xffffffffLL));
}

Int64 Map::TileMapLayer::CoordToId(Math::Coord2D<Int32> tileId)
{
	return (((Int64)(UInt32)tileId.x) << 32) | (UInt32)tileId.y;
}

void Map::TileMapLayer::AddTask(CachedImage *cimg)
{
	Sync::MutexUsage mutUsage(&this->taskMut);
	this->taskQueued.Put(cimg);
	mutUsage.EndUse();
	this->threads[this->threadNext].evt->Set();
	this->threadNext = (this->threadNext + 1) % this->threadCnt;
}

void Map::TileMapLayer::CheckCache(Data::ArrayListInt64 *currIDs)
{
	if (this->lastIds.GetCount() <= 0)
		return;

	CachedImage *cimg;
	Data::ArrayListInt64 cacheIds;
	Data::ArrayList<CachedImage *> cacheImgs;
	UOSInt i;
	OSInt j;
	UOSInt k;
	Sync::MutexUsage lastMutUsage(&this->lastMut);
	cacheIds.AddAll(&this->lastIds);
	cacheImgs.AddAll(&this->lastImgs);
	this->lastIds.Clear();
	this->lastImgs.Clear();

	i = currIDs->GetCount();
	while (i-- > 0)
	{
		j = cacheIds.SortedIndexOf(currIDs->GetItem(i));
		if (j >= 0)
		{
			k = this->lastIds.SortedInsert(cacheIds.RemoveAt((UOSInt)j));
			this->lastImgs.Insert(k, cimg = cacheImgs.RemoveAt((UOSInt)j));
		}
	}
	lastMutUsage.EndUse();

	Sync::MutexUsage idleMutUsage(&this->idleMut);
	i = cacheImgs.GetCount();
	while (i-- > 0)
	{
		cimg = cacheImgs.GetItem(i);
		if (cimg->isFinish)
		{
			if (cimg->img)
			{
				DEL_CLASS(cimg->img);
			}
			MemFreeA(cimg);
		}
		else
		{
			cimg->isCancel = true;
			this->idleImgs.Add(cimg);
		}
	}

	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItem(i);
		if (cimg->isFinish)
		{
			this->idleImgs.RemoveAt(i);
			if (cimg->img)
			{
				DEL_CLASS(cimg->img);
			}
			MemFreeA(cimg);
		}
	}
	idleMutUsage.EndUse();
}

Map::TileMapLayer::TileMapLayer(Map::TileMap *tileMap, Parser::ParserList *parsers) : Map::MapDrawLayer(tileMap->GetName(), 0, CSTR_NULL)
{
	this->parsers = parsers;
	this->tileMap = tileMap;
	this->scale = 10000;

	this->lastLevel = (UOSInt)-1;
	this->threadNext = 0;
	this->csys = tileMap->GetCoordinateSystem()->Clone();

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
		Sync::ThreadUtil::Create(TaskThread, &this->threads[i]);
	}
	Bool running = false;
	while (!running)
	{
		running = true;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (!this->threads[i].running)
			{
				running = false;
				break;
			}
		}
		if (!running)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
}

Map::TileMapLayer::~TileMapLayer()
{
	CachedImage *cimg;
	UOSInt i;
	Bool running;
	Sync::MutexUsage mutUsage(&this->updMut);
	this->updHdlrs.Clear();
	this->updObjs.Clear();
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
		Sync::SimpleThread::Sleep(1);
	}
	MemFree(this->threads);

	i = this->lastImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->lastImgs.GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFreeA(cimg);
	}
	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFreeA(cimg);
	}
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
		Sync::MutexUsage lastMutUsage(&this->lastMut);
		Sync::MutexUsage idleMutUsage(&this->idleMut);
		this->lastLevel = level;
		j = this->lastImgs.GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs.GetItem(j);
			if (cimg->isFinish)
			{
				if (cimg->img)
				{
					DEL_CLASS(cimg->img);
				}
				MemFreeA(cimg);
			}
			else
			{
				cimg->isCancel = true;
				this->idleImgs.Add(cimg);
			}
		}
		this->lastIds.Clear();
		this->lastImgs.Clear();
		idleMutUsage.EndUse();
		lastMutUsage.EndUse();
	}
}

Map::MapView *Map::TileMapLayer::CreateMapView(Math::Size2DDbl scnSize)
{
	Map::MapView *view;
	if (this->tileMap->IsMercatorProj())
	{
		NEW_CLASS(view, Map::MercatorMapView(scnSize, Math::Coord2DDbl(114.2, 22.4), this->tileMap->GetLevelCount(), this->tileMap->GetTileSize()));
		return view;
	}
	else
	{
		Data::ArrayListDbl scales;
		UOSInt i = 0;
		UOSInt j = this->tileMap->GetLevelCount();
		while (i < j)
		{
			scales.Add(this->tileMap->GetLevelScale(i));
			i++;
		}
		Math::RectAreaDbl bounds;
		this->tileMap->GetBounds(&bounds);
		Math::CoordinateSystem *csys = this->tileMap->GetCoordinateSystem();
		NEW_CLASS(view, Map::LeveledMapView(csys && csys->IsProjected(), scnSize, bounds.GetCenter(), &scales));
		return view;
	}
}

Map::DrawLayerType Map::TileMapLayer::GetLayerType()
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::TileMapLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr)
{
	UOSInt retCnt;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (nameArr)
		*nameArr = 0;
	Data::ArrayList<Math::Coord2D<Int32>> idArr;
	retCnt = this->tileMap->GetTileImageIDs(level, Math::RectAreaDbl(Math::Coord2DDbl(-180, -90), Math::Coord2DDbl(180, 90)), &idArr);
	UOSInt i = 0;
	while (i < retCnt)
	{
		outArr->Add(CoordToId(idArr.GetItem(i)));
		i++;
	}
	i = this->tileMap->GetScreenObjCnt();
	retCnt += i;
	while (i-- > 0)
	{
		outArr->Add(CoordToId(Math::Coord2D<Int32>((Int32)0x80000000, (Int32)i)));
	}
	return retCnt;
}

UOSInt Map::TileMapLayer::GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	UOSInt retCnt;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (nameArr)
		*nameArr = 0;
	Data::ArrayList<Math::Coord2D<Int32>> idArr;
	retCnt = this->tileMap->GetTileImageIDs(level, rect.ToDouble() / mapRate, &idArr);
	UOSInt i = 0;
	while (i < retCnt)
	{
		outArr->Add(CoordToId(idArr.GetItem(i)));
		i++;
	}
	CheckCache(outArr);
	i = this->tileMap->GetScreenObjCnt();
	retCnt += i;
	while (i-- > 0)
	{
		outArr->Add(CoordToId(Math::Coord2D<Int32>((Int32)0x80000000, (Int32)i)));
	}
	return retCnt;
}

UOSInt Map::TileMapLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt retCnt;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (nameArr)
		*nameArr = 0;
	Data::ArrayList<Math::Coord2D<Int32>> idArr;
	retCnt = this->tileMap->GetTileImageIDs(level, rect, &idArr);
	UOSInt i = 0;
	while (i < retCnt)
	{
		outArr->Add(CoordToId(idArr.GetItem(i)));
		i++;
	}
	CheckCache(outArr);
	i = this->tileMap->GetScreenObjCnt();
	retCnt += i;
	while (i-- > 0)
	{
		outArr->Add(CoordToId(Math::Coord2D<Int32>((Int32)0x80000000, (Int32)i)));
	}
	return retCnt;
}

Int64 Map::TileMapLayer::GetObjectIdMax()
{
	return 0;
}

void Map::TileMapLayer::ReleaseNameArr(NameArray *nameArr)
{
}

UTF8Char *Map::TileMapLayer::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	switch (strIndex)
	{
	case 0:
		return Text::StrInt64(buff, id);
	case 1:
		return Text::StrUOSInt(buff, this->tileMap->GetNearestLevel(scale));
	case 2:
	{
		Math::Coord2D<Int32> tileId = IdToCoord(id);
		if (tileId.x == (Int32)0x80000000)
		{
			return this->tileMap->GetScreenObjURL(buff, (UInt32)tileId.y);
		}
		else
		{
			return this->tileMap->GetTileImageURL(buff, this->tileMap->GetNearestLevel(scale), IdToCoord(id));
		}
	}
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
		return Text::StrConcatC(buff, UTF8STRC("id"));
	case 1:
		return Text::StrConcatC(buff, UTF8STRC("level"));
	case 2:
		return Text::StrConcatC(buff, UTF8STRC("url"));
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
		return DB::DBUtil::CT_VarUTF8Char;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::TileMapLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt colSize;
	switch (colIndex)
	{
	case 0:
		colDef->SetPK(true);
	case 1:
	case 2:
		sptr = this->GetColumnName(sbuff, colIndex);
		colDef->SetColName(CSTRP(sbuff, sptr));
		colDef->SetColType(this->GetColumnType(colIndex, &colSize));
		colDef->SetColSize(colSize);
		colDef->SetColDP(0);
		colDef->SetNotNull(true);
		colDef->SetAutoIncNone();
		return true;
	}
	return false;
}

UInt32 Map::TileMapLayer::GetCodePage()
{
	return 0;
}

Bool Map::TileMapLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	return this->tileMap->GetBounds(bounds);
}

Map::GetObjectSess *Map::TileMapLayer::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::TileMapLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::TileMapLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	CachedImage *cimg;
	Math::Geometry::VectorImage *vimg;
	OSInt i;
	UOSInt k;
	Media::ImageList *imgList;
	Math::RectAreaDbl bounds;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	Math::Coord2D<Int32> tileId = IdToCoord(id);
	if (tileId.x == (Int32)0x80000000)
	{
		return this->tileMap->CreateScreenObjVector((UInt32)tileId.y);
	}
	i = this->lastIds.SortedIndexOf(id);
	if (i >= 0)
	{
		cimg = this->lastImgs.GetItem((UOSInt)i);
		if (cimg->img == 0)
			return 0;
		sptr = this->tileMap->GetTileImageURL(sbuff, cimg->level, tileId);
		NEW_CLASS(vimg, Math::Geometry::VectorImage(this->csys->GetSRID(), cimg->img, cimg->tl, cimg->br, false, {sbuff, (UOSInt)(sptr - sbuff)}, 0, 0));
		return vimg;
	}

	if (this->IsCaching(level, id))
	{
		return 0;
	}
	imgList = this->tileMap->LoadTileImage(level, tileId, this->parsers, &bounds, true);
	if (imgList)
	{
		cimg = MemAllocA(CachedImage, 1);
		cimg->imgId = id;
		cimg->tl = bounds.tl;
		cimg->br = bounds.br;
		cimg->level = level;
		cimg->isFinish = true;
		cimg->isCancel = false;
		NEW_CLASS(cimg->img, Media::SharedImage(imgList, false));

		Sync::MutexUsage mutUsage(&this->lastMut);
		k = this->lastIds.SortedInsert(id);
		this->lastImgs.Insert(k, cimg);
		mutUsage.EndUse();

		sptr = this->tileMap->GetTileImageURL(sbuff, level, tileId);
		NEW_CLASS(vimg, Math::Geometry::VectorImage(this->csys->GetSRID(), cimg->img, cimg->tl, cimg->br, false, {sbuff, (UOSInt)(sptr - sbuff)}, 0, 0));
		return vimg;
	}
	else
	{
		cimg = MemAllocA(CachedImage, 1);
		cimg->imgId = id;
		cimg->tl = bounds.tl;;
		cimg->br = bounds.br;
		cimg->level = level;
		cimg->isFinish = false;
		cimg->isCancel = false;
		cimg->img = 0;
		AddTask(cimg);

		Sync::MutexUsage mutUsage(&this->lastMut);
		k = this->lastIds.SortedInsert(id);
		this->lastImgs.Insert(k, cimg);
		mutUsage.EndUse();

		return 0;
	}
}

Map::MapDrawLayer::ObjectClass Map::TileMapLayer::GetObjectClass()
{
	return Map::MapDrawLayer::OC_TILE_MAP_LAYER;
}

Bool Map::TileMapLayer::CanQuery()
{
	return this->tileMap->CanQuery();
}

Bool Map::TileMapLayer::QueryInfos(Math::Coord2DDbl coord, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	return this->tileMap->QueryInfos(coord, level, vecList, valueOfstList, nameList, valueList);
}

void Map::TileMapLayer::AddUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(&this->updMut);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
	mutUsage.EndUse();
}

void Map::TileMapLayer::RemoveUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(&this->updMut);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs.GetItem(i) == hdlr && this->updObjs.GetItem(i) == obj)
		{
			this->updObjs.RemoveAt(i);
			this->updHdlrs.RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

Bool Map::TileMapLayer::IsCaching(UOSInt level, Int64 imgId)
{
	UOSInt i;
	Bool found = false;
	CachedImage *cimg;
	Sync::MutexUsage mutUsage(&this->idleMut);
	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItem(i);
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
		Sync::SimpleThread::Sleep(10);
	}
}

Map::TileMap *Map::TileMapLayer::GetTileMap()
{
	return this->tileMap;
}
