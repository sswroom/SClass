#include "Stdafx.h"
#include "Map/MapLayerCollection.h"
#include "Math/CoordinateSystemManager.h"
#include "Sync/RWMutexUsage.h"

void __stdcall Map::MapLayerCollection::InnerUpdated(AnyType userObj)
{
	NotNullPtr<Map::MapLayerCollection> me = userObj.GetNN<Map::MapLayerCollection>();
	UOSInt i;
	Sync::RWMutexUsage mutUsage(me->mut, false);
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = me->updHdlrs.GetItem(i);
		cb.func(cb.userObj);
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
	if (this->layerList.GetCount() == 0)
	{
		this->SetCoordinateSystem(val->GetCoordinateSystem()->Clone());
	}
	Sync::RWMutexUsage mutUsage(this->mut, true);
	return this->layerList.Add(val);
}

Optional<Map::MapDrawLayer> Map::MapLayerCollection::RemoveAt(UOSInt index)
{
	NotNullPtr<Map::MapDrawLayer> lyr;
	Sync::RWMutexUsage mutUsage(this->mut, true);
	if (this->layerList.RemoveAt(index).SetTo(lyr))
	{
		lyr->RemoveUpdatedHandler(InnerUpdated, this);
		return lyr;
	}
	return 0;
}

void Map::MapLayerCollection::Clear()
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		it.Next()->RemoveUpdatedHandler(InnerUpdated, this);
	}
	this->layerList.Clear();
}

UOSInt Map::MapLayerCollection::GetCount() const
{
	return this->layerList.GetCount();
}

Optional<Map::MapDrawLayer> Map::MapLayerCollection::GetItem(UOSInt Index) const
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	return this->layerList.GetItem(Index);
}

Data::ArrayIterator<NotNullPtr<Map::MapDrawLayer>> Map::MapLayerCollection::Iterator(NotNullPtr<Sync::RWMutexUsage> mutUsage) const
{
	mutUsage->ReplaceMutex(this->mut, false);
	return this->layerList.Iterator();
}

void Map::MapLayerCollection::SetItem(UOSInt Index, NotNullPtr<Map::MapDrawLayer> val)
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	this->layerList.SetItem(Index, val);
}

void Map::MapLayerCollection::SetCurrScale(Double scale)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetCurrScale(scale);
	}
}

void Map::MapLayerCollection::SetCurrTimeTS(Int64 timeStamp)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetCurrTimeTS(timeStamp);
	}
}

Int64 Map::MapLayerCollection::GetTimeStartTS() const
{
	Int64 timeStart = 0;
	Int64 v;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		v = it.Next()->GetTimeStartTS();
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
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		v = it.Next()->GetTimeEndTS();
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
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		NotNullPtr<MapDrawLayer> layer = it.Next();
		if (lyrType == Map::DRAW_LAYER_UNKNOWN)
		{
			lyrType = layer->GetLayerType();
		}
		else if (lyrType != layer->GetLayerType())
		{
			lyrType = Map::DRAW_LAYER_MIXED;
		}
	}
	return lyrType;
}

UOSInt Map::MapLayerCollection::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	NotNullPtr<Map::MapDrawLayer> lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	ret = 0;
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
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
	}
	return ret;
}

UOSInt Map::MapLayerCollection::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	NotNullPtr<Map::MapDrawLayer> lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
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
	}
	return ret;
}

UOSInt Map::MapLayerCollection::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	NotNullPtr<Map::MapDrawLayer> lyr;
	Data::ArrayListInt64 tmpArr;
	UOSInt m1;
	UOSInt m2;
	UOSInt ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
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
	}
	return ret;
}

Int64 Map::MapLayerCollection::GetObjectIdMax() const
{
	NotNullPtr<Map::MapDrawLayer> lyr;
	Int64 maxId;
	Int64 currId = 0;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
		maxId = lyr->GetObjectIdMax();
		currId += maxId + 1;
	}
	return currId - 1;
}

void Map::MapLayerCollection::ReleaseNameArr(NameArray *nameArr)
{
}

Bool Map::MapLayerCollection::GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	Int64 currId = 0;
	Int64 maxId;
	NotNullPtr<Map::MapDrawLayer> lyr;
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
		maxId = lyr->GetObjectIdMax();
		if (id >= currId && id <= currId + maxId)
		{
			return lyr->GetString(sb, 0, id - currId, strIndex);
		}
		currId += maxId + 1;
	}
	return false;
}

UOSInt Map::MapLayerCollection::GetColumnCnt() const
{
	return 0;
}

UTF8Char *Map::MapLayerCollection::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType Map::MapLayerCollection::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
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
	Math::RectAreaDbl minMax;
	Math::RectAreaDbl thisBounds;

	Sync::RWMutexUsage mutUsage(NotNullPtr<Sync::RWMutex>::ConvertFrom(NotNullPtr<const Sync::RWMutex>(this->mut)), false);
	NotNullPtr<Map::MapDrawLayer> lyr;
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
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
				minMax.min = minMax.min.Min(thisBounds.min);
				minMax.max = minMax.max.Max(thisBounds.max);
			}
		}
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
	Int64 currId = 0;
	Int64 maxId;
	NotNullPtr<Map::MapDrawLayer> lyr;
	Math::Geometry::Vector2D *vec = 0;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
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
	}
	return vec;
}

void Map::MapLayerCollection::AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	this->updHdlrs.Add({hdlr, obj});
}

void Map::MapLayerCollection::RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(this->mut, true);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = this->updHdlrs.GetItem(i);
		if (cb.func == hdlr && cb.userObj == obj)
		{
			this->updHdlrs.RemoveAt(i);
		}
	}
}

Map::MapDrawLayer::ObjectClass Map::MapLayerCollection::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_MAP_LAYER_COLL;
}

NotNullPtr<Math::CoordinateSystem> Map::MapLayerCollection::GetCoordinateSystem()
{
	NotNullPtr<MapDrawLayer> lyr;
	if (this->layerList.GetItem(0).SetTo(lyr))
		return lyr->GetCoordinateSystem();
	else
		return this->csys;
}

void Map::MapLayerCollection::SetCoordinateSystem(NotNullPtr<Math::CoordinateSystem> csys)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NotNullPtr<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetCoordinateSystem(csys->Clone());
	}
	this->csys.Delete();
	this->csys = csys;
}

void Map::MapLayerCollection::ReleaseAll()
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.RemoveAt(i).Delete();
	}
}

UOSInt Map::MapLayerCollection::GetUpdatedHandlerCnt() const
{
	return this->updHdlrs.GetCount();
}

Map::MapDrawLayer::UpdatedHandler Map::MapLayerCollection::GetUpdatedHandler(UOSInt index) const
{
	return this->updHdlrs.GetItem(index).func;
}

AnyType Map::MapLayerCollection::GetUpdatedObject(UOSInt index) const
{
	return this->updHdlrs.GetItem(index).userObj;
}
