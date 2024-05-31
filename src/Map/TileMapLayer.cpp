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

UInt32 __stdcall Map::TileMapLayer::TaskThread(AnyType userObj)
{
	Math::RectAreaDbl bounds;
	NN<Media::ImageList> imgList;
	NN<Media::SharedImage> shimg;
	CachedImage *cimg;
	NN<ThreadStat> stat = userObj.GetNN<ThreadStat>();
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("TileMapLayer")), stat->index);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->running = true;
		stat->isIdle = false;
		while (!stat->toStop)
		{
			while (!stat->toStop)
			{
				Sync::MutexUsage mutUsage(stat->me->taskMut);
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
					if (imgList.Set(stat->me->tileMap->LoadTileImage(cimg->level, IdToCoord(cimg->imgId), stat->me->parsers, bounds, false)))
					{
						NEW_CLASSNN(shimg, Media::SharedImage(imgList, false));
						cimg->img = shimg;
						cimg->isFinish = true;
						
						Sync::MutexUsage mutUsage(stat->me->updMut);
						UOSInt i = stat->me->updHdlrs.GetCount();
						while (i-- > 0)
						{
							Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = stat->me->updHdlrs.GetItem(i);
							cb.func(cb.userObj);
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

void Map::TileMapLayer::AddTask(NN<CachedImage> cimg)
{
	Sync::MutexUsage mutUsage(this->taskMut);
	this->taskQueued.Put(cimg.Ptr());
	mutUsage.EndUse();
	this->threads[this->threadNext].evt->Set();
	this->threadNext = (this->threadNext + 1) % this->threadCnt;
}

void Map::TileMapLayer::CheckCache(NN<Data::ArrayListInt64> currIDs)
{
	if (this->lastIds.GetCount() <= 0)
		return;

	NN<CachedImage> cimg;
	Data::ArrayListInt64 cacheIds;
	Data::ArrayListNN<CachedImage> cacheImgs;
	UOSInt i;
	OSInt j;
	UOSInt k;
	Sync::MutexUsage lastMutUsage(this->lastMut);
	cacheIds.AddAll(this->lastIds);
	cacheImgs.AddAll(this->lastImgs);
	this->lastIds.Clear();
	this->lastImgs.Clear();

	i = currIDs->GetCount();
	while (i-- > 0)
	{
		j = cacheIds.SortedIndexOf(currIDs->GetItem(i));
		if (j >= 0)
		{
			k = this->lastIds.SortedInsert(cacheIds.RemoveAt((UOSInt)j));
			this->lastImgs.Insert(k, cacheImgs.GetItemNoCheck((UOSInt)j));
			cacheImgs.RemoveAt((UOSInt)j);
		}
	}
	lastMutUsage.EndUse();

	Sync::MutexUsage idleMutUsage(this->idleMut);
	i = cacheImgs.GetCount();
	while (i-- > 0)
	{
		cimg = cacheImgs.GetItemNoCheck(i);
		if (cimg->isFinish)
		{
			cimg->img.Delete();
			MemFreeANN(cimg);
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
		cimg = this->idleImgs.GetItemNoCheck(i);
		if (cimg->isFinish)
		{
			this->idleImgs.RemoveAt(i);
			cimg->img.Delete();
			MemFreeANN(cimg);
		}
	}
	idleMutUsage.EndUse();
}

Map::TileMapLayer::TileMapLayer(NN<Map::TileMap> tileMap, NN<Parser::ParserList> parsers) : Map::MapDrawLayer(tileMap->GetName(), 0, CSTR_NULL, tileMap->GetCoordinateSystem()->Clone())
{
	this->parsers = parsers;
	this->tileMap = tileMap;
	this->scale = 10000;

	this->lastLevel = (UOSInt)-1;
	this->threadNext = 0;

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
		this->threads[i].index = i;
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
	NN<CachedImage> cimg;
	UOSInt i;
	Bool running;
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Clear();
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
		cimg = this->lastImgs.GetItemNoCheck(i);
		cimg->img.Delete();
		MemFreeANN(cimg);
	}
	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItemNoCheck(i);
		cimg->img.Delete();
		MemFreeANN(cimg);
	}
	this->tileMap.Delete();
}

void Map::TileMapLayer::SetCurrScale(Double scale)
{
	NN<CachedImage> cimg;
	UOSInt j;

	this->scale = scale;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	if (this->lastLevel != level)
	{
		Sync::MutexUsage lastMutUsage(this->lastMut);
		Sync::MutexUsage idleMutUsage(this->idleMut);
		this->lastLevel = level;
		j = this->lastImgs.GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs.GetItemNoCheck(j);
			if (cimg->isFinish)
			{
				cimg->img.Delete();
				MemFreeANN(cimg);
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

NN<Map::MapView> Map::TileMapLayer::CreateMapView(Math::Size2DDbl scnSize)
{
	NN<Map::MapView> view;
	if (this->tileMap->IsMercatorProj())
	{
		NEW_CLASSNN(view, Map::MercatorMapView(scnSize, Math::Coord2DDbl(114.2, 22.4), this->tileMap->GetMaxLevel(), this->tileMap->GetTileSize()));
		return view;
	}
	else
	{
		Data::ArrayListDbl scales;
		UOSInt i = this->tileMap->GetMinLevel();
		UOSInt j = this->tileMap->GetMaxLevel();
		while (i <= j)
		{
			scales.Add(this->tileMap->GetLevelScale(i));
			i++;
		}
		Math::RectAreaDbl bounds;
		this->tileMap->GetBounds(bounds);
		NN<Math::CoordinateSystem> csys = this->tileMap->GetCoordinateSystem();
		NEW_CLASSNN(view, Map::LeveledMapView(csys->IsProjected(), scnSize, bounds.GetCenter(), scales));
		return view;
	}
}

Map::DrawLayerType Map::TileMapLayer::GetLayerType() const
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::TileMapLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr)
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

UOSInt Map::TileMapLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
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

UOSInt Map::TileMapLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
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

Int64 Map::TileMapLayer::GetObjectIdMax() const
{
	return 0;
}

void Map::TileMapLayer::ReleaseNameArr(NameArray *nameArr)
{
}

Bool Map::TileMapLayer::GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	switch (strIndex)
	{
	case 0:
		sb->AppendI64(id);
		return true;
	case 1:
		sb->AppendUOSInt(this->tileMap->GetNearestLevel(scale));
		return true;
	case 2:
	{
		Math::Coord2D<Int32> tileId = IdToCoord(id);
		if (tileId.x == (Int32)0x80000000)
		{
			return this->tileMap->GetScreenObjURL(sb, (UInt32)tileId.y);
		}
		else
		{
			return this->tileMap->GetTileImageURL(sb, this->tileMap->GetNearestLevel(scale), IdToCoord(id));
		}
	}
	}
	return false;
}

UOSInt Map::TileMapLayer::GetColumnCnt() const
{
	return 3;
}

UnsafeArrayOpt<UTF8Char> Map::TileMapLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
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

DB::DBUtil::ColType Map::TileMapLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	switch (colIndex)
	{
	case 0:
		colSize.Set(20);
		return DB::DBUtil::CT_Int64;
	case 1:
		colSize.Set(10);
		return DB::DBUtil::CT_UInt32;
	case 2:
		colSize.Set(512);
		return DB::DBUtil::CT_VarUTF8Char;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::TileMapLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt colSize;
	switch (colIndex)
	{
	case 0:
		colDef->SetPK(true);
	case 1:
	case 2:
		sptr = this->GetColumnName(sbuff, colIndex).Or(sbuff);
		colDef->SetColName(CSTRP(sbuff, sptr));
		colDef->SetColType(this->GetColumnType(colIndex, colSize));
		colDef->SetColSize(colSize);
		colDef->SetColDP(0);
		colDef->SetNotNull(true);
		colDef->SetAutoIncNone();
		return true;
	}
	return false;
}

UInt32 Map::TileMapLayer::GetCodePage() const
{
	return 0;
}

Bool Map::TileMapLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
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
	NN<CachedImage> cimg;
	Math::Geometry::VectorImage *vimg;
	OSInt i;
	UOSInt k;
	Media::ImageList *imgList;
	NN<Media::ImageList> nnimgList;
	Math::RectAreaDbl bounds;
	NN<Media::SharedImage> shimg;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	Math::Coord2D<Int32> tileId = IdToCoord(id);
	if (tileId.x == (Int32)0x80000000)
	{
		return this->tileMap->CreateScreenObjVector((UInt32)tileId.y).OrNull();
	}
	i = this->lastIds.SortedIndexOf(id);
	if (i >= 0)
	{
		cimg = this->lastImgs.GetItemNoCheck((UOSInt)i);
		if (!cimg->img.SetTo(shimg))
			return 0;
		sptr = this->tileMap->GetTileImageURL(sbuff, cimg->level, tileId).Or(sbuff);
		NEW_CLASS(vimg, Math::Geometry::VectorImage(this->csys->GetSRID(), shimg, cimg->tl, cimg->br, false, {sbuff, (UOSInt)(sptr - sbuff)}, 0, 0));
		return vimg;
	}

	if (this->IsCaching(level, id))
	{
		return 0;
	}
	imgList = this->tileMap->LoadTileImage(level, tileId, this->parsers, bounds, true);
	if (nnimgList.Set(imgList))
	{
		cimg = MemAllocANN(CachedImage);
		cimg->imgId = id;
		cimg->tl = bounds.min;
		cimg->br = bounds.max;
		cimg->level = level;
		cimg->isFinish = true;
		cimg->isCancel = false;
		NEW_CLASSNN(shimg, Media::SharedImage(nnimgList, false));
		cimg->img = shimg;

		Sync::MutexUsage mutUsage(this->lastMut);
		k = this->lastIds.SortedInsert(id);
		this->lastImgs.Insert(k, cimg);
		mutUsage.EndUse();

		sptr = this->tileMap->GetTileImageURL(sbuff, level, tileId).Or(sbuff);
		NEW_CLASS(vimg, Math::Geometry::VectorImage(this->csys->GetSRID(), shimg, cimg->tl, cimg->br, false, {sbuff, (UOSInt)(sptr - sbuff)}, 0, 0));
		return vimg;
	}
	else
	{
		cimg = MemAllocANN(CachedImage);
		cimg->imgId = id;
		cimg->tl = bounds.min;;
		cimg->br = bounds.max;
		cimg->level = level;
		cimg->isFinish = false;
		cimg->isCancel = false;
		cimg->img = 0;
		AddTask(cimg);

		Sync::MutexUsage mutUsage(this->lastMut);
		k = this->lastIds.SortedInsert(id);
		this->lastImgs.Insert(k, cimg);
		mutUsage.EndUse();

		return 0;
	}
}

Map::MapDrawLayer::ObjectClass Map::TileMapLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_TILE_MAP_LAYER;
}

Bool Map::TileMapLayer::CanQuery()
{
	return this->tileMap->CanQuery();
}

Bool Map::TileMapLayer::QueryInfos(Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
{
	UOSInt level = this->tileMap->GetNearestLevel(scale);
	return this->tileMap->QueryInfos(coord, level, vecList, valueOfstList, nameList, valueList);
}

void Map::TileMapLayer::AddUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Add({hdlr, obj});
}

void Map::TileMapLayer::RemoveUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = this->updHdlrs.GetItem(i);
		if (cb.func == hdlr && cb.userObj == obj)
		{
			this->updHdlrs.RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

Bool Map::TileMapLayer::IsCaching(UOSInt level, Int64 imgId)
{
	UOSInt i;
	Bool found = false;
	NN<CachedImage> cimg;
	Sync::MutexUsage mutUsage(this->idleMut);
	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItemNoCheck(i);
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

NN<Map::TileMap> Map::TileMapLayer::GetTileMap()
{
	return this->tileMap;
}
