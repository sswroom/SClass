#include "Stdafx.h"
#include "Map/MapLayerCollection.h"
#include "Math/CoordinateSystemManager.h"
#include "Sync/RWMutexUsage.h"

void __stdcall Map::MapLayerCollection::InnerUpdated(void *userObj)
{
	Map::MapLayerCollection *me = (Map::MapLayerCollection*)userObj;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(me->mut, false);
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		me->updHdlrs.GetItem(i)(me->updObjs.GetItem(i));
	}
}

Map::MapLayerCollection::MapLayerCollection(NotNullPtr<Text::String> sourceName, Text::String *layerName) : Map::MapDrawLayer(sourceName, 0, layerName, Math::CoordinateSystemManager::CreateDefaultCsys())
{
}

Map::MapLayerCollection::MapLayerCollection(Text::CStringNN sourceName, Text::CString layerName) : Map::MapDrawLayer(sourceName, 0, layerName, Math::CoordinateSystemManager::CreateDefaultCsys())
{
}

Map::MapLayerCollection::~MapLayerCollection()
{
	this->ReleaseAll();
}

UOSInt Map::MapLayerCollection::Add(NotNullPtr<Map::MapDrawLayer> val)
{
	val->AddUpdatedHandler(InnerUpdated, this);
	Sync::RWMutexUsage mutUsage(this->mut, true);
	return this->layerList.Add(val);
}

Map::MapDrawLayer *Map::MapLayerCollection::RemoveAt(UOSInt index)
{
	Map::MapDrawLayer *lyr;
	Sync::RWMutexUsage mutUsage(this->mut, true);
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
	Sync::RWMutexUsage mutUsage(this->mut, true);
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

Map::MapDrawLayer *Map::MapLayerCollection::GetItem(UOSInt Index)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	return this->layerList.GetItem(Index);
}

void Map::MapLayerCollection::SetItem(UOSInt Index, NotNullPtr<Map::MapDrawLayer> val)
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	this->layerList.SetItem(Index, val);
}

void Map::MapLayerCollection::SetCurrScale(Double scale)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCurrScale(scale);
	}
}

void Map::MapLayerCollection::SetCurrTimeTS(Int64 timeStamp)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCurrTimeTS(timeStamp);
	}
}

Int64 Map::MapLayerCollection::GetTimeStartTS() const
{
	Int64 timeStart = 0;
	Int64 v;
	Sync::RWMutexUsage mutUsage(this->mut, false);
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

Int64 Map::MapLayerCollection::GetTimeEndTS() const
{
	Int64 timeEnd = 0;
	Int64 v;
	Sync::RWMutexUsage mutUsage(this->mut, false);
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

Map::DrawLayerType Map::MapLayerCollection::GetLayerType() const
{
	Map::DrawLayerType lyrType = Map::DRAW_LAYER_UNKNOWN;
	Sync::RWMutexUsage mutUsage(this->mut, false);
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

UOSInt Map::MapLayerCollection::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	Map::MapDrawLayer *lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt k;
	UOSInt l;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	ret = 0;
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetAllObjectIds(tmpArr, nameArr);
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

UOSInt Map::MapLayerCollection::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	Map::MapDrawLayer *lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt k;
	UOSInt l;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetObjectIds(tmpArr, nameArr, mapRate, rect, keepEmpty);
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

UOSInt Map::MapLayerCollection::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Map::MapDrawLayer *lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt k;
	UOSInt l;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetObjectIdsMapXY(tmpArr, nameArr, rect, keepEmpty);
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

Int64 Map::MapLayerCollection::GetObjectIdMax() const
{
	UOSInt k;
	UOSInt l;
	Map::MapDrawLayer *lyr;
	Int64 maxId;
	Int64 currId = 0;
	Sync::RWMutexUsage mutUsage(this->mut, false);
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

void Map::MapLayerCollection::ReleaseNameArr(NameArray *nameArr)
{
}

UTF8Char *Map::MapLayerCollection::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	Map::MapDrawLayer *lyr;
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

UOSInt Map::MapLayerCollection::GetColumnCnt() const
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

Bool Map::MapLayerCollection::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	return false;
}

UInt32 Map::MapLayerCollection::GetCodePage() const
{
	return 0;
}

Bool Map::MapLayerCollection::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	Bool isFirst = true;
	UOSInt k;
	UOSInt l;
	Math::RectAreaDbl minMax;
	Math::RectAreaDbl thisBounds;

	Sync::RWMutexUsage mutUsage(NotNullPtr<Sync::RWMutex>::ConvertFrom(NotNullPtr<const Sync::RWMutex>(this->mut)), false);
	Map::MapDrawLayer *lyr;
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		if (isFirst)
		{
			if (lyr->GetBounds(minMax))
			{
				isFirst = false;
			}
		}
		else
		{
			if (lyr->GetBounds(thisBounds))
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
		bounds.Set(minMax);
		return true;
	}
}

Map::GetObjectSess *Map::MapLayerCollection::BeginGetObject()
{
	return (GetObjectSess*)this;
}

void Map::MapLayerCollection::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::MapLayerCollection::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	Map::MapDrawLayer *lyr;
	Math::Geometry::Vector2D *vec = 0;
	Sync::RWMutexUsage mutUsage(this->mut, false);
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

void Map::MapLayerCollection::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
}

void Map::MapLayerCollection::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(this->mut, true);
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

Map::MapDrawLayer::ObjectClass Map::MapLayerCollection::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_MAP_LAYER_COLL;
}

NotNullPtr<Math::CoordinateSystem> Map::MapLayerCollection::GetCoordinateSystem()
{
	if (this->layerList.GetCount() > 0)
		return this->layerList.GetItem(0)->GetCoordinateSystem();
	else
		return this->csys;
}

void Map::MapLayerCollection::SetCoordinateSystem(NotNullPtr<Math::CoordinateSystem> csys)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCoordinateSystem(csys->Clone());
	}
	this->csys.Delete();
	this->csys = csys;
}

void Map::MapLayerCollection::ReleaseAll()
{
	Map::MapDrawLayer *lyr;
	Sync::RWMutexUsage mutUsage(this->mut, true);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		lyr = this->layerList.RemoveAt(i);
		DEL_CLASS(lyr);
	}
}

UOSInt Map::MapLayerCollection::GetUpdatedHandlerCnt() const
{
	return this->updHdlrs.GetCount();
}

Map::MapDrawLayer::UpdatedHandler Map::MapLayerCollection::GetUpdatedHandler(UOSInt index) const
{
	return this->updHdlrs.GetItem(index);
}

void *Map::MapLayerCollection::GetUpdatedObject(UOSInt index) const
{
	return this->updObjs.GetItem(index);
}
