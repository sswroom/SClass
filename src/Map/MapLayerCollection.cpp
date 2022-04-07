#include "Stdafx.h"
#include "Map/MapLayerCollection.h"

void __stdcall Map::MapLayerCollection::InnerUpdated(void *userObj)
{
	Map::MapLayerCollection *me = (Map::MapLayerCollection*)userObj;
	UOSInt i;
	me->mut.LockRead();
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		me->updHdlrs.GetItem(i)(me->updObjs.GetItem(i));
	}
	me->mut.UnlockRead();
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
	UOSInt i;
	val->AddUpdatedHandler(InnerUpdated, this);
	this->mut.LockWrite();
	i = this->layerList.Add(val);
	this->mut.UnlockWrite();
	return i;
}

Map::IMapDrawLayer *Map::MapLayerCollection::RemoveAt(UOSInt index)
{
	Map::IMapDrawLayer *lyr;
	this->mut.LockWrite();
	lyr = this->layerList.RemoveAt(index);
	if (lyr)
	{
		lyr->RemoveUpdatedHandler(InnerUpdated, this);
	}
	this->mut.UnlockWrite();
	return lyr;
}

void Map::MapLayerCollection::Clear()
{
	UOSInt i;
	this->mut.LockWrite();
	i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->RemoveUpdatedHandler(InnerUpdated, this);
	}
	this->layerList.Clear();
	this->mut.UnlockWrite();
}

UOSInt Map::MapLayerCollection::GetCount() const
{
	return this->layerList.GetCount();
}

Map::IMapDrawLayer *Map::MapLayerCollection::GetItem(UOSInt Index)
{
	Map::IMapDrawLayer *lyr;
	this->mut.LockRead();
	lyr = this->layerList.GetItem(Index);
	this->mut.UnlockRead();
	return lyr;
}

void Map::MapLayerCollection::SetItem(UOSInt Index, Map::IMapDrawLayer *Val)
{
	this->mut.LockWrite();
	this->layerList.SetItem(Index, Val);
	this->mut.UnlockWrite();
}

void Map::MapLayerCollection::SetCurrScale(Double scale)
{
	this->mut.LockRead();
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCurrScale(scale);
	}
	this->mut.UnlockRead();
}

void Map::MapLayerCollection::SetCurrTimeTS(Int64 timeStamp)
{
	this->mut.LockRead();
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCurrTimeTS(timeStamp);
	}
	this->mut.UnlockRead();
}

Int64 Map::MapLayerCollection::GetTimeStartTS()
{
	Int64 timeStart = 0;
	Int64 v;
	this->mut.LockRead();
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
	this->mut.UnlockRead();
	return timeStart;
}

Int64 Map::MapLayerCollection::GetTimeEndTS()
{
	Int64 timeEnd = 0;
	Int64 v;
	this->mut.LockRead();
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
	this->mut.UnlockRead();
	return timeEnd;
}

Map::DrawLayerType Map::MapLayerCollection::GetLayerType()
{
	Map::DrawLayerType lyrType = Map::DRAW_LAYER_UNKNOWN;
	this->mut.LockRead();
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
	this->mut.UnlockRead();
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
	this->mut.LockRead();
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
	this->mut.UnlockRead();
	return ret;
}

UOSInt Map::MapLayerCollection::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
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
	this->mut.LockRead();
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetObjectIds(&tmpArr, nameArr, mapRate, x1, y1, x2, y2, keepEmpty);
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
	this->mut.UnlockRead();
	return ret;
}

UOSInt Map::MapLayerCollection::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
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
	this->mut.LockRead();
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		tmpArr.Clear();
		m2 = lyr->GetObjectIdsMapXY(&tmpArr, nameArr, x1, y1, x2, y2, keepEmpty);
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
	this->mut.UnlockRead();
	return ret;
}

Int64 Map::MapLayerCollection::GetObjectIdMax()
{
	UOSInt k;
	UOSInt l;
	Map::IMapDrawLayer *lyr;
	Int64 maxId;
	Int64 currId = 0;
	this->mut.LockRead();
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		maxId = lyr->GetObjectIdMax();
		currId += maxId + 1;
		k++;
	}
	this->mut.UnlockRead();
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

Bool Map::MapLayerCollection::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	Bool isFirst = true;
	UOSInt k;
	UOSInt l;
	Double currMaxX;
	Double currMaxY;
	Double currMinX;
	Double currMinY;
	Double thisMaxX;
	Double thisMaxY;
	Double thisMinX;
	Double thisMinY;

	this->mut.LockRead();
	Map::IMapDrawLayer *lyr;
	k = 0;
	l = this->layerList.GetCount();
	while (k < l)
	{
		lyr = this->layerList.GetItem(k);
		if (isFirst)
		{
			if (lyr->GetBoundsDbl(&currMinX, &currMinY, &currMaxX, &currMaxY))
			{
				isFirst = false;
			}
		}
		else
		{
			if (lyr->GetBoundsDbl(&thisMinX, &thisMinY, &thisMaxX, &thisMaxY))
			{
				if (thisMinX < currMinX)
					currMinX = thisMinX;
				if (thisMinY < currMinY)
					currMinY = thisMinY;
				if (thisMaxX > currMaxX)
					currMaxX = thisMaxX;
				if (thisMaxY > currMaxY)
					currMaxY = thisMaxY;
			}
		}
		k++;
	}
	this->mut.UnlockRead();
	if (isFirst)
	{
		return false;
	}
	else
	{
		*minX = currMinX;
		*minY = currMinY;
		*maxX = currMaxX;
		*maxY = currMaxY;
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
	this->mut.LockRead();
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
	this->mut.UnlockRead();
	return dobj;
}

Math::Vector2D *Map::MapLayerCollection::GetNewVectorById(void *session, Int64 id)
{
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	Map::IMapDrawLayer *lyr;
	Math::Vector2D *vec = 0;
	this->mut.LockRead();
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
	this->mut.UnlockRead();
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
	this->mut.LockWrite();
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
	this->mut.UnlockWrite();
}

void Map::MapLayerCollection::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	this->mut.LockWrite();
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs.GetItem(i) == hdlr && this->updObjs.GetItem(i) == obj)
		{
			this->updHdlrs.RemoveAt(i);
			this->updObjs.RemoveAt(i);
		}
	}
	this->mut.UnlockWrite();
}

Map::IMapDrawLayer::ObjectClass Map::MapLayerCollection::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_MAP_LAYER_COLL;
}

Math::CoordinateSystem *Map::MapLayerCollection::GetCoordinateSystem()
{
	Math::CoordinateSystem *csys = 0;
	this->mut.LockRead();
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
	this->mut.UnlockRead();
	return csys;
}

void Map::MapLayerCollection::SetCoordinateSystem(Math::CoordinateSystem *csys)
{
	this->mut.LockRead();
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		this->layerList.GetItem(i)->SetCoordinateSystem(csys);
	}
	this->mut.UnlockRead();
}

void Map::MapLayerCollection::ReleaseAll()
{
	Map::IMapDrawLayer *lyr;
	this->mut.LockWrite();
	UOSInt i = this->layerList.GetCount();
	while (i-- > 0)
	{
		lyr = this->layerList.RemoveAt(i);
		DEL_CLASS(lyr);
	}
	this->mut.UnlockWrite();
}
