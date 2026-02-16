#include "Stdafx.h"
#include "Map/MapLayerCollection.h"
#include "Math/CoordinateSystemManager.h"
#include "Sync/RWMutexUsage.h"

void __stdcall Map::MapLayerCollection::InnerUpdated(AnyType userObj)
{
	NN<Map::MapLayerCollection> me = userObj.GetNN<Map::MapLayerCollection>();
	UIntOS i;
	Sync::RWMutexUsage mutUsage(me->mut, false);
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = me->updHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

Map::MapLayerCollection::MapLayerCollection(NN<Text::String> sourceName, Text::String *layerName) : Map::MapDrawLayer(sourceName, 0, layerName, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->failReason = Map::MapDrawLayer::FailReason::IdNotFound;
}

Map::MapLayerCollection::MapLayerCollection(Text::CStringNN sourceName, Text::CString layerName) : Map::MapDrawLayer(sourceName, 0, layerName, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->failReason = Map::MapDrawLayer::FailReason::IdNotFound;
}

Map::MapLayerCollection::~MapLayerCollection()
{
	this->ReleaseAll();
}

UIntOS Map::MapLayerCollection::Add(NN<Map::MapDrawLayer> val)
{
	val->AddUpdatedHandler(InnerUpdated, this);
	if (this->layerList.GetCount() == 0)
	{
		this->SetCoordinateSystem(val->GetCoordinateSystem()->Clone());
	}
	Sync::RWMutexUsage mutUsage(this->mut, true);
	return this->layerList.Add(val);
}

Optional<Map::MapDrawLayer> Map::MapLayerCollection::RemoveAt(UIntOS index)
{
	NN<Map::MapDrawLayer> lyr;
	Sync::RWMutexUsage mutUsage(this->mut, true);
	if (this->layerList.RemoveAt(index).SetTo(lyr))
	{
		lyr->RemoveUpdatedHandler(InnerUpdated, this);
		return lyr;
	}
	return nullptr;
}

void Map::MapLayerCollection::Clear()
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		it.Next()->RemoveUpdatedHandler(InnerUpdated, this);
	}
	this->layerList.Clear();
}

UIntOS Map::MapLayerCollection::GetCount() const
{
	return this->layerList.GetCount();
}

Optional<Map::MapDrawLayer> Map::MapLayerCollection::GetItem(UIntOS Index) const
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	return this->layerList.GetItem(Index);
}

Data::ArrayIterator<NN<Map::MapDrawLayer>> Map::MapLayerCollection::Iterator(NN<Sync::RWMutexUsage> mutUsage) const
{
	mutUsage->ReplaceMutex(this->mut, false);
	return this->layerList.Iterator();
}

void Map::MapLayerCollection::SetItem(UIntOS Index, NN<Map::MapDrawLayer> val)
{
	Sync::RWMutexUsage mutUsage(this->mut, true);
	this->layerList.SetItem(Index, val);
}

void Map::MapLayerCollection::SetCurrScale(Double scale)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetCurrScale(scale);
	}
}

void Map::MapLayerCollection::SetCurrTimeTS(Int64 timeStamp)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		NN<MapDrawLayer> layer = it.Next();
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

UIntOS Map::MapLayerCollection::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	NN<Map::MapDrawLayer> lyr;
	Data::ArrayListInt64 tmpArr;
	UIntOS m1;
	UIntOS m2;
	UIntOS ret;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	ret = 0;
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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

UIntOS Map::MapLayerCollection::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	NN<Map::MapDrawLayer> lyr;
	Data::ArrayListInt64 tmpArr;
	UIntOS m1;
	UIntOS m2;
	UIntOS ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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

UIntOS Map::MapLayerCollection::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	NN<Map::MapDrawLayer> lyr;
	Data::ArrayListInt64 tmpArr;
	UIntOS m1;
	UIntOS m2;
	UIntOS ret = 0;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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
	NN<Map::MapDrawLayer> lyr;
	Int64 maxId;
	Int64 currId = 0;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
		maxId = lyr->GetObjectIdMax();
		currId += maxId + 1;
	}
	return currId - 1;
}

UIntOS Map::MapLayerCollection::GetRecordCnt() const
{
	NN<Map::MapDrawLayer> lyr;
	UIntOS cnt = 0;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
		cnt += lyr->GetRecordCnt();
	}
	return cnt;
}

void Map::MapLayerCollection::ReleaseNameArr(Optional<NameArray> nameArr)
{
}

Bool Map::MapLayerCollection::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex)
{
	Int64 currId = 0;
	Int64 maxId;
	NN<Map::MapDrawLayer> lyr;
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
		maxId = lyr->GetObjectIdMax();
		if (id >= currId && id <= currId + maxId)
		{
			return lyr->GetString(sb, nullptr, id - currId, strIndex);
		}
		currId += maxId + 1;
	}
	return false;
}

UIntOS Map::MapLayerCollection::GetColumnCnt() const
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> Map::MapLayerCollection::GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const
{
	return nullptr;
}

DB::DBUtil::ColType Map::MapLayerCollection::GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::MapLayerCollection::GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const
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

	Sync::RWMutexUsage mutUsage(NN<Sync::RWMutex>::ConvertFrom(NN<const Sync::RWMutex>(this->mut)), false);
	NN<Map::MapDrawLayer> lyr;
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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

NN<Map::GetObjectSess> Map::MapLayerCollection::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<MapLayerCollection>(*this));
}

void Map::MapLayerCollection::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::MapLayerCollection::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	Int64 currId = 0;
	Int64 maxId;
	NN<Map::MapDrawLayer> lyr;
	Optional<Math::Geometry::Vector2D> vec = nullptr;
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	this->failReason = Map::MapDrawLayer::FailReason::IdNotFound;
	while (it.HasNext())
	{
		lyr = it.Next();
		maxId = lyr->GetObjectIdMax();
		if (id >= currId && id <= currId + maxId)
		{
			vec = lyr->GetNewVectorById(session, id - currId);
			this->failReason = lyr->GetFailReason();
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
	UIntOS i;
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

Map::MapDrawLayer::FailReason Map::MapLayerCollection::GetFailReason() const
{
	return this->failReason;
}

void Map::MapLayerCollection::WaitForLoad(Data::Duration maxWaitTime)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
	while (it.HasNext())
	{
		it.Next()->WaitForLoad(maxWaitTime);
	}
}

UIntOS Map::MapLayerCollection::GetGeomCol() const
{
	NN<Map::MapDrawLayer> layer;
	if (this->layerList.GetItem(0).SetTo(layer))
	{
		return layer->GetGeomCol();
	}
	return INVALID_INDEX;
}

Map::MapDrawLayer::ObjectClass Map::MapLayerCollection::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_MAP_LAYER_COLL;
}

NN<Math::CoordinateSystem> Map::MapLayerCollection::GetCoordinateSystem() const
{
	NN<MapDrawLayer> lyr;
	if (this->layerList.GetItem(0).SetTo(lyr))
		return lyr->GetCoordinateSystem();
	else
		return this->csys;
}

void Map::MapLayerCollection::SetCoordinateSystem(NN<Math::CoordinateSystem> csys)
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	Data::ArrayIterator<NN<MapDrawLayer>> it = this->layerList.Iterator();
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
	UIntOS i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.RemoveAt(i).Delete();
	}
}

void Map::MapLayerCollection::ReorderLayers()
{
	UIntOS imageIndex = 0;
	UIntOS areaIndex = 0;
	UIntOS lineIndex = 0;
	UIntOS pointIndex = 0;
	NN<Map::MapDrawLayer> layer;
	Map::DrawLayerType type;
	UIntOS i = 0;
	UIntOS j = this->layerList.GetCount();
	while (i < j)
	{
		layer = this->layerList.GetItemNoCheck(i);
		type = layer->GetLayerType();
		if (type == Map::DRAW_LAYER_IMAGE)
		{
			if (imageIndex != i)
			{
				this->layerList.RemoveAt(i);
				this->layerList.Insert(imageIndex, layer);
			}
			imageIndex++;
			areaIndex++;
			lineIndex++;
			pointIndex++;
		}
		else if (type == Map::DRAW_LAYER_POLYGON || type == Map::DRAW_LAYER_MIXED)
		{
			if (areaIndex != i)
			{
				this->layerList.RemoveAt(i);
				this->layerList.Insert(areaIndex, layer);
			}
			areaIndex++;
			lineIndex++;
			pointIndex++;
		}
		else if (type == Map::DRAW_LAYER_POLYLINE3D || type == Map::DRAW_LAYER_POLYLINE)
		{
			if (lineIndex != i)
			{
				this->layerList.RemoveAt(i);
				this->layerList.Insert(lineIndex, layer);
			}
			lineIndex++;
			pointIndex++;
		}
		else
		{
			pointIndex++;
		}
		i++;
	}
}

UIntOS Map::MapLayerCollection::GetUpdatedHandlerCnt() const
{
	return this->updHdlrs.GetCount();
}

Map::MapDrawLayer::UpdatedHandler Map::MapLayerCollection::GetUpdatedHandler(UIntOS index) const
{
	return this->updHdlrs.GetItem(index).func;
}

AnyType Map::MapLayerCollection::GetUpdatedObject(UIntOS index) const
{
	return this->updHdlrs.GetItem(index).userObj;
}
