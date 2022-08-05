#include "Stdafx.h"
#include "Map/MapLayerCollection.h"
#include "Sync/RWMutexUsage.h"

void __stdcall Map::MapLayerCollection::InnerUpdated(void *userObj)
{
	Map::MapLayerCollection *me = (Map::MapLayerCollection*)userObj;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&me->mut, false);
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		me->updHdlrs.GetItem(i)(me->updObjs.GetItem(i));
	}
}

Map::MapLayerCollection::MapLayerCollection(Text::String *sourceName, Text::String *layerName) : Map::IMapDrawLayer(sourceName, 0, layerName)
{
}

Map::MapLayerCollection::MapLayerCollection(Text::CString sourceName, Text::CString layerName) : Map::IMapDrawLayer(sourceName, 0, layerName)
{
}

Map::MapLayerCollection::~MapLayerCollection()
{
	this->ReleaseAll();
}

UOSInt Map::MapLayerCollection::Add(Map::IMapDrawLayer * val)
{
	val->AddUpdatedHandler(InnerUpdated, this);
	Sync::RWMutexUsage mutUsage(&this->mut, true);
	return this->layerList.Add(val);
}

Map::IMapDrawLayer *Map::MapLayerCollection::RemoveAt(UOSInt index)
{
	Map::IMapDrawLayer *lyr;
	Sync::RWMutexUsage mutUsage(&this->mut, true);
	lyr = this->layerList.RemoveAt(index);
	if (lyr)
	{
		lyr->RemoveUpdatedHandler(InnerUpdated, this);
	}
	return lyr;
}

void Map::MapLayerCollection::Clear()
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->mut, true);
	i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->RemoveUpdatedHandler(InnerUpdated, this);
	}
	this->layerList.Clear();
}

UOSInt Map::MapLayerCollection::GetCount() const
{
	return this->layerList.GetCount();
}

Map::IMapDrawLayer *Map::MapLayerCollection::GetItem(UOSInt Index)
{
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	return this->layerList.GetItem(Index);
}

void Map::MapLayerCollection::SetItem(UOSInt Index, Map::IMapDrawLayer *Val)
{
	Sync::RWMutexUsage mutUsage(&this->mut, true);
	this->layerList.SetItem(Index, Val);
}

void Map::MapLayerCollection::SetCurrScale(Double scale)
{
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCurrScale(scale);
	}
}

void Map::MapLayerCollection::SetCurrTimeTS(Int64 timeStamp)
{
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCurrTimeTS(timeStamp);
	}
}

Int64 Map::MapLayerCollection::GetTimeStartTS()
{
	Int64 timeStart = 0;
	Int64 v;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		v = this->layerList.GetItem(i)->GetTimeStartTS();
		if (timeStart == 0)
		{
			timeStart = v;
		}
		else if (v != 0 && v < timeStart)
		{
			timeStart = v;
		}
	}
	return timeStart;
}

Int64 Map::MapLayerCollection::GetTimeEndTS()
{
	Int64 timeEnd = 0;
	Int64 v;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		v = this->layerList.GetItem(i)->GetTimeEndTS();
		if (timeEnd == 0)
		{
			timeEnd = v;
		}
		else if (v != 0 && v > timeEnd)
		{
			timeEnd = v;
		}
	}
	return timeEnd;
}

Map::DrawLayerType Map::MapLayerCollection::GetLayerType()
{
	Map::DrawLayerType lyrType = Map::DRAW_LAYER_UNKNOWN;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		if (lyrType == Map::DRAW_LAYER_UNKNOWN)
		{
			lyrType = this->layerList.GetItem(i)->GetLayerType();
		}
		else if (lyrType != this->layerList.GetItem(i)->GetLayerType())
		{
			lyrType = Map::DRAW_LAYER_MIXED;
		}
	}
	return lyrType;
}

UOSInt Map::MapLayerCollection::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	Map::IMapDrawLayer *lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt k;
	UOSInt l;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	ret = 0;
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetAllObjectIds(&tmpArr, nameArr);
		m1 = 0;
		while (m1 < m2)
		{
			outArr->Add(tmpArr.GetItem(m1) + currId);
			m1++;
		}
		ret += m2;
		currId += maxId + 1;
		k++;
	}
	return ret;
}

UOSInt Map::MapLayerCollection::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	Map::IMapDrawLayer *lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt k;
	UOSInt l;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetObjectIds(&tmpArr, nameArr, mapRate, rect, keepEmpty);
		m1 = 0;
		while (m1 < m2)
		{
			outArr->Add(tmpArr.GetItem(m1) + currId);
			m1++;
		}
		ret += m2;
		currId += maxId + 1;
		k++;
	}
	return ret;
}

UOSInt Map::MapLayerCollection::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Map::IMapDrawLayer *lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt k;
	UOSInt l;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetObjectIdsMapXY(&tmpArr, nameArr, rect, keepEmpty);
		m1 = 0;
		while (m1 < m2)
		{
			outArr->Add(tmpArr.GetItem(m1) + currId);
			m1++;
		}
		ret += m2;
		currId += maxId + 1;
		k++;
	}
	return ret;
}

Int64 Map::MapLayerCollection::GetObjectIdMax()
{
	UOSInt k;
	UOSInt l;
	Map::IMapDrawLayer *lyr;
	Int64 maxId;
	Int64 currId = 0;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		currId += maxId + 1;
		k++;
	}
	return currId - 1;
}

void Map::MapLayerCollection::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::MapLayerCollection::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	Map::IMapDrawLayer *lyr;
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		if (id >= currId && id <= currId + maxId)
		{
			return lyr->GetString(buff, buffSize, 0, id - currId, strIndex);
		}
		currId += maxId + 1;
		k++;
	}
	return 0;
}

UOSInt Map::MapLayerCollection::GetColumnCnt()
{
	return 0;
}

UTF8Char *Map::MapLayerCollection::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType Map::MapLayerCollection::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::MapLayerCollection::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return false;
}

UInt32 Map::MapLayerCollection::GetCodePage()
{
	return 0;
}

Bool Map::MapLayerCollection::GetBounds(Math::RectAreaDbl *bounds)
{
	Bool isFirst = true;
	UOSInt k;
	UOSInt l;
	Math::RectAreaDbl minMax;
	Math::RectAreaDbl thisBounds;

	Sync::RWMutexUsage mutUsage(&this->mut, false);
	Map::IMapDrawLayer *lyr;
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		if (isFirst)
		{
			if (lyr->GetBounds(&minMax))
			{
				isFirst = false;
			}
		}
		else
		{
			if (lyr->GetBounds(&thisBounds))
			{
				minMax.tl = minMax.tl.Min(thisBounds.tl);
				minMax.br = minMax.br.Max(thisBounds.br);
			}
		}
		k++;
	}
	if (isFirst)
	{
		return false;
	}
	else
	{
		*bounds = minMax;
		return true;
	}
}

void *Map::MapLayerCollection::BeginGetObject()
{
	return this;
}

void Map::MapLayerCollection::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::MapLayerCollection::GetNewObjectById(void *session, Int64 id)
{
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	Map::IMapDrawLayer *lyr;
	Map::DrawObjectL *dobj = 0;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		if (id >= currId && id <= currId + maxId)
		{
			dobj = lyr->GetNewObjectById(session, id - currId);
			break;
		}
		else
		{
			currId += maxId + 1;
		}
		k++;
	}
	return dobj;
}

Math::Geometry::Vector2D *Map::MapLayerCollection::GetNewVectorById(void *session, Int64 id)
{
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	Map::IMapDrawLayer *lyr;
	Math::Geometry::Vector2D *vec = 0;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		if (id >= currId && id <= currId + maxId)
		{
			vec = lyr->GetNewVectorById(session, id - currId);
			break;
		}
		else
		{
			currId += maxId + 1;
		}
		k++;
	}
	return vec;
}

void Map::MapLayerCollection::ReleaseObject(void *session, DrawObjectL *obj)
{
	if (obj->ptOfstArr)
		MemFree(obj->ptOfstArr);
	if (obj->pointArr)
		MemFree(obj->pointArr);
	MemFree(obj);
}

void Map::MapLayerCollection::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	Sync::RWMutexUsage mutUsage(&this->mut, true);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
}

void Map::MapLayerCollection::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->mut, true);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs.GetItem(i) == hdlr && this->updObjs.GetItem(i) == obj)
		{
			this->updHdlrs.RemoveAt(i);
			this->updObjs.RemoveAt(i);
		}
	}
}

Map::IMapDrawLayer::ObjectClass Map::MapLayerCollection::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_MAP_LAYER_COLL;
}

Math::CoordinateSystem *Map::MapLayerCollection::GetCoordinateSystem()
{
	Math::CoordinateSystem *csys = 0;
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	UOSInt i = 0;
	UOSInt j = this->layerList.GetCount();
	while (i < j)
	{
		csys = this->layerList.GetItem(i)->GetCoordinateSystem();
		if (csys)
		{
			break;
		}
		i++;
	}
	return csys;
}

void Map::MapLayerCollection::SetCoordinateSystem(Math::CoordinateSystem *csys)
{
	Sync::RWMutexUsage mutUsage(&this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCoordinateSystem(csys);
	}
}

void Map::MapLayerCollection::ReleaseAll()
{
	Map::IMapDrawLayer *lyr;
	Sync::RWMutexUsage mutUsage(&this->mut, true);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		lyr = this->layerList.RemoveAt(i);
		DEL_CLASS(lyr);
	}
}
