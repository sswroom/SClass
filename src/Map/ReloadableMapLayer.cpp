#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/ReloadableMapLayer.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"

void __stdcall Map::ReloadableMapLayer::InnerUpdated(void *userObj)
{
	Map::ReloadableMapLayer *me = (Map::ReloadableMapLayer*)userObj;
	OSInt i;
	me->innerLayerMut->LockRead();
	i = me->updHdlrs->GetCount();
	while (i-- > 0)
	{
		me->updHdlrs->GetItem(i)(me->updObjs->GetItem(i));
	}
	me->innerLayerMut->UnlockRead();
}

Map::ReloadableMapLayer::ReloadableMapLayer(const UTF8Char *fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, const UTF8Char *layerName) : Map::IMapDrawLayer(fileName, 0, layerName)
{
	this->parsers = parsers;
	this->browser = browser;
	NEW_CLASS(this->innerLayerMut, Sync::RWMutex());
	NEW_CLASS(this->innerLayers, Data::ArrayList<InnerLayerInfo*>());
	this->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	this->currScale = 10000;
	this->currTime = 0;
	NEW_CLASS(this->updHdlrs, Data::ArrayList<UpdatedHandler>());
	NEW_CLASS(this->updObjs, Data::ArrayList<void*>());
}

Map::ReloadableMapLayer::~ReloadableMapLayer()
{
	OSInt i;
	InnerLayerInfo *innerLayer;
	DEL_CLASS(this->innerLayerMut);
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		innerLayer = this->innerLayers->GetItem(i);
		SDEL_CLASS(innerLayer->innerLayer);
		SDEL_TEXT(innerLayer->url);
		SDEL_TEXT(innerLayer->layerName);
		MemFree(innerLayer);
	}
	DEL_CLASS(this->innerLayers);
	DEL_CLASS(this->updHdlrs);
	DEL_CLASS(this->updObjs);
}

void Map::ReloadableMapLayer::SetCurrScale(Double scale)
{
	OSInt i;
	this->innerLayerMut->LockRead();
	this->currScale = scale;
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->SetCurrScale(scale);
		}
	}
	this->innerLayerMut->UnlockRead();
}

void Map::ReloadableMapLayer::SetCurrTimeTS(Int64 timeStamp)
{
	OSInt i;
	this->innerLayerMut->LockRead();
	this->currTime = timeStamp;
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->SetCurrTimeTS(timeStamp);
		}
	}
	this->innerLayerMut->UnlockRead();
}

Int64 Map::ReloadableMapLayer::GetTimeStartTS()
{
	Bool first = true;
	Int64 timeStart = 0;
	Int64 v;
	OSInt i;
	this->innerLayerMut->LockRead();
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			if (first)
			{
				timeStart = innerLayer->innerLayer->GetTimeStartTS();
				first = (timeStart == 0);
			}
			else
			{
				v = innerLayer->innerLayer->GetTimeStartTS();
				if (v != 0 && v < timeStart)
				{
					timeStart = v;
				}
			}
		}
	}
	this->innerLayerMut->UnlockRead();
	return timeStart;
}

Int64 Map::ReloadableMapLayer::GetTimeEndTS()
{
	Bool first = true;
	Int64 timeEnd = 0;
	Int64 v;
	OSInt i;
	this->innerLayerMut->LockRead();
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			if (first)
			{
				timeEnd = innerLayer->innerLayer->GetTimeEndTS();
				first = (timeEnd == 0);
			}
			else
			{
				v = innerLayer->innerLayer->GetTimeEndTS();
				if (v != 0 && v > timeEnd)
				{
					timeEnd = v;
				}
			}
		}
	}
	this->innerLayerMut->UnlockRead();
	return timeEnd;
}

Map::DrawLayerType Map::ReloadableMapLayer::GetLayerType()
{
	return this->innerLayerType;
}

UOSInt Map::ReloadableMapLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	this->innerLayerMut->LockRead();
	i = 0;
	j = this->innerLayers->GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = innerLayer->innerLayer->GetAllObjectIds(&tmpArr, nameArr);
			k = 0;
			while (k < l)
			{
				outArr->Add(tmpArr.GetItem(k) + currId);
				k++;
			}
			ret += l;
			currId += maxId + 1;
		}
		i++;
	}

	this->innerLayerMut->UnlockRead();
	return ret;
}

UOSInt Map::ReloadableMapLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	this->innerLayerMut->LockRead();
	i = 0;
	j = this->innerLayers->GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = innerLayer->innerLayer->GetObjectIds(&tmpArr, nameArr, mapRate, x1, y1, x2, y2, keepEmpty);
			k = 0;
			while (k < l)
			{
				outArr->Add(tmpArr.GetItem(k) + currId);
				k++;
			}
			ret += l;
			currId += maxId + 1;
		}
		i++;
	}

	this->innerLayerMut->UnlockRead();
	return ret;
}

UOSInt Map::ReloadableMapLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	this->innerLayerMut->LockRead();
	i = 0;
	j = this->innerLayers->GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = innerLayer->innerLayer->GetObjectIdsMapXY(&tmpArr, nameArr, x1, y1, x2, y2, keepEmpty);
			k = 0;
			while (k < l)
			{
				outArr->Add(tmpArr.GetItem(k) + currId);
				k++;
			}
			ret += l;
			currId += maxId + 1;
		}
		i++;
	}

	this->innerLayerMut->UnlockRead();
	return ret;
}

Int64 Map::ReloadableMapLayer::GetObjectIdMax()
{
	OSInt i;
	Int64 currId = 0;
	Int64 maxId;
	this->innerLayerMut->LockRead();
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			currId += maxId + 1;
		}
	}
	this->innerLayerMut->UnlockRead();
	return currId - 1;
}
void Map::ReloadableMapLayer::ReleaseNameArr(void *nameArr)
{
/*	Map::IMapDrawLayer *lyr = this->innerLayer;
	if (lyr)
		lyr->ReleaseNameArr(nameArr);*/
}

UTF8Char *Map::ReloadableMapLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	UOSInt i;
	UOSInt j;
	Int64 currId = 0;
	Int64 maxId;
	i = 0;
	j = this->innerLayers->GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			if (id >= currId && id <= currId + maxId)
			{
				return innerLayer->innerLayer->GetString(buff, buffSize, 0, id - currId, strIndex);
			}
			currId += maxId + 1;
		}
		i++;
	}
	return 0;
}

UOSInt Map::ReloadableMapLayer::GetColumnCnt()
{
	return 0;
}

UTF8Char *Map::ReloadableMapLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType Map::ReloadableMapLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::ReloadableMapLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return false;
}

Int32 Map::ReloadableMapLayer::GetCodePage()
{
	return 0;
}

Bool Map::ReloadableMapLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	Bool isFirst = true;
	OSInt i;
	Double currMaxX;
	Double currMaxY;
	Double currMinX;
	Double currMinY;
	Double thisMaxX;
	Double thisMaxY;
	Double thisMinX;
	Double thisMinY;
	this->innerLayerMut->LockRead();
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			if (isFirst)
			{
				if (innerLayer->innerLayer->GetBoundsDbl(&currMinX, &currMinY, &currMaxX, &currMaxY))
				{
					isFirst = false;
				}
			}
			else
			{
				if (innerLayer->innerLayer->GetBoundsDbl(&thisMinX, &thisMinY, &thisMaxX, &thisMaxY))
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
		}
	}
	this->innerLayerMut->UnlockRead();
	if (isFirst)
	{
		*minX = 0;
		*minY = 0;
		*maxX = 0;
		*maxY = 0;
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

void *Map::ReloadableMapLayer::BeginGetObject()
{
	OSInt i;
	this->innerLayerMut->LockRead();
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->sess = innerLayer->innerLayer->BeginGetObject();
		}
	}
	return this;
}
void Map::ReloadableMapLayer::EndGetObject(void *session)
{
	OSInt i;
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->EndGetObject(innerLayer->sess);
		}
	}
	this->innerLayerMut->UnlockRead();
}

Map::DrawObjectL *Map::ReloadableMapLayer::GetObjectByIdD(void *session, Int64 id)
{
	OSInt i;
	OSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Map::DrawObjectL *dobj = 0;
	i = 0;
	this->innerLayerMut->LockRead();
	j = this->innerLayers->GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			if (id >= currId && id <= currId + maxId)
			{
				dobj = innerLayer->innerLayer->GetObjectByIdD(session, id - currId);
				break;
			}
			else
			{
				currId += maxId + 1;
			}
		}
		i++;
	}
	this->innerLayerMut->UnlockRead();
	return dobj;
}

Math::Vector2D *Map::ReloadableMapLayer::GetVectorById(void *session, Int64 id)
{
	OSInt i;
	OSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Math::Vector2D *vec = 0;
	i = 0;
	this->innerLayerMut->LockRead();
	j = this->innerLayers->GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			if (id >= currId && id <= currId + maxId)
			{
				vec = innerLayer->innerLayer->GetVectorById(session, id - currId);
				break;
			}
			else
			{
				currId += maxId + 1;
			}
		}
		i++;
	}
	this->innerLayerMut->UnlockRead();
	return vec;
}

void Map::ReloadableMapLayer::ReleaseObject(void *session, DrawObjectL *obj)
{
	if (obj->ptOfstArr)
		MemFree(obj->ptOfstArr);
	if (obj->pointArr)
		MemFree(obj->pointArr);
	MemFree(obj);
}

UOSInt Map::ReloadableMapLayer::GetNameCol()
{
	return 0;
}
void Map::ReloadableMapLayer::SetNameCol(UOSInt nameCol)
{
}

Map::IMapDrawLayer::ObjectClass Map::ReloadableMapLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_RELOADABLE_LAYER;
}

Math::CoordinateSystem *Map::ReloadableMapLayer::GetCoordinateSystem()
{
	Math::CoordinateSystem *csys = 0;
	OSInt i;
	this->innerLayerMut->LockRead();
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer;
		innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			csys = innerLayer->innerLayer->GetCoordinateSystem();
			if (csys)
				break;
		}
	}
	if (csys == 0)
		csys = this->csys;
	this->innerLayerMut->UnlockRead();
	return csys;
}

void Map::ReloadableMapLayer::SetCoordinateSystem(Math::CoordinateSystem *csys)
{
	OSInt i;
	this->innerLayerMut->LockWrite();
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->SetCoordinateSystem(csys->Clone());
		}
	}
	SDEL_CLASS(this->csys);
	this->csys = csys;
	this->innerLayerMut->UnlockWrite();
}

void Map::ReloadableMapLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	OSInt i;
	InnerLayerInfo *innerLayer;
	this->innerLayerMut->LockWrite();
	this->updHdlrs->Add(hdlr);
	this->updObjs->Add(obj);
	i = this->innerLayers->GetCount();
	while (i-- > 0)
	{
		innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->AddUpdatedHandler(hdlr, obj);
		}
	}
	this->innerLayerMut->UnlockWrite();
}

void Map::ReloadableMapLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	OSInt i;
	InnerLayerInfo *innerLayer;
	Bool chg = false;
	this->innerLayerMut->LockWrite();
	i = this->updHdlrs->GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs->GetItem(i) == hdlr && this->updObjs->GetItem(i) == obj)
		{
			this->updHdlrs->RemoveAt(i);
			this->updObjs->RemoveAt(i);
			chg = true;
		}
	}
	if (chg)
	{
		i = this->innerLayers->GetCount();
		while (i-- > 0)
		{
			innerLayer = this->innerLayers->GetItem(i);
			if (innerLayer->innerLayer)
			{
				innerLayer->innerLayer->RemoveUpdatedHandler(hdlr, obj);
			}
		}
	}
	this->innerLayerMut->UnlockWrite();
}

void Map::ReloadableMapLayer::AddInnerLayer(const UTF8Char *name, const UTF8Char *url, Int32 seconds)
{
	Text::StringBuilderUTF8 sb;
	OSInt urlLen;
	sb.Append(this->GetSourceNameObj());
	urlLen = Text::StrCharCnt(url);
	sb.AllocLeng(urlLen);
	sb.SetEndPtr(Text::URLString::AppendURLPath(sb.ToString(), url));
	InnerLayerInfo *innerLayer;
	innerLayer = MemAlloc(InnerLayerInfo, 1);
	innerLayer->innerLayer = 0;
	innerLayer->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	innerLayer->url = Text::StrCopyNew(sb.ToString());
	if (name)
	{
		innerLayer->layerName = Text::StrCopyNew(name);
	}
	else
	{
		innerLayer->layerName = 0;
	}
	innerLayer->reloadInterval = seconds;
	this->innerLayerMut->LockWrite();
	this->innerLayers->Add(innerLayer);
	this->innerLayerMut->UnlockWrite();
	this->Reload();
}

/*
void Map::ReloadableMapLayer::SetLayerName(const WChar *name)
{
	if (name == 0)
		return;
	if (this->layerName)
	{
		Text::StrDelNew(this->layerName);
	}
	this->layerName = Text::StrCopyNew(name);
}

void Map::ReloadableMapLayer::SetReloadURL(const WChar *url)
{
	if (url == 0)
		return;
	this->innerLayerMut->Lock();
	if (this->url)
	{
		Text::StrDelNew(this->url);
	}
	this->url = Text::StrCopyNew(url);
	this->innerLayerMut->Unlock();
	this->Reload();
}

void Map::ReloadableMapLayer::SetReloadInterval(Int32 seconds)
{
	this->reloadInterval = seconds;
}*/

void Map::ReloadableMapLayer::Reload()
{
	OSInt i;
	this->innerLayerMut->LockRead();
	i = this->innerLayers->GetCount();
	this->innerLayerMut->UnlockRead();
	while (i-- > 0)
	{
        IO::IStreamData *data = 0;
		InnerLayerInfo *innerLayer;
		this->innerLayerMut->LockRead();
		innerLayer = this->innerLayers->GetItem(i);
		if (innerLayer->innerLayer == 0 || innerLayer->reloadInterval != 0)
		{
			data = this->browser->GetData(innerLayer->url, innerLayer->reloadInterval != 0, 0);
		}
		this->innerLayerMut->UnlockRead();
		if (data)
		{
			while (data->IsLoading())
			{
				Sync::Thread::Sleep(10);
			}
			IO::ParsedObject::ParserType pt;
			IO::ParsedObject *pobj = this->parsers->ParseFile(data, &pt);
			DEL_CLASS(data);
			if (pobj)
			{
				if (pt == IO::ParsedObject::PT_MAP_LAYER_PARSER)
				{
					OSInt j;
					this->innerLayerMut->LockWrite();
					SDEL_CLASS(innerLayer->innerLayer);
					innerLayer->innerLayer = (Map::IMapDrawLayer*)pobj;
					innerLayer->innerLayerType = innerLayer->innerLayer->GetLayerType();
					innerLayer->innerLayer->AddUpdatedHandler(InnerUpdated, this);
					j = this->updHdlrs->GetCount();
					while (j-- > 0)
					{
						innerLayer->innerLayer->AddUpdatedHandler(this->updHdlrs->GetItem(j), this->updObjs->GetItem(j));
					}
					this->innerLayerMut->UnlockWrite();
					if (this->innerLayerType != innerLayer->innerLayerType)
					{
						if (this->innerLayerType == Map::DRAW_LAYER_UNKNOWN)
						{
							this->innerLayerType = innerLayer->innerLayerType;
						}
						else if (innerLayer->innerLayerType == Map::DRAW_LAYER_UNKNOWN || innerLayer->innerLayerType == this->innerLayerType)
						{
						}
						else
						{
							this->innerLayerType = Map::DRAW_LAYER_MIXED;
						}
					}

					this->innerLayerMut->LockRead();
					j = this->updHdlrs->GetCount();
					while (j-- > 0)
					{
						this->updHdlrs->GetItem(j)(this->updObjs->GetItem(j));
					}
					this->innerLayerMut->UnlockRead();
				}
				else
				{
					DEL_CLASS(pobj);
				}
			}
		}

	}
}
