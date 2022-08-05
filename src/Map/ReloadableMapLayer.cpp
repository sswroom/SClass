#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/ReloadableMapLayer.h"
#include "Sync/RWMutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"

void __stdcall Map::ReloadableMapLayer::InnerUpdated(void *userObj)
{
	Map::ReloadableMapLayer *me = (Map::ReloadableMapLayer*)userObj;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&me->innerLayerMut, false);
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		me->updHdlrs.GetItem(i)(me->updObjs.GetItem(i));
	}
}

Map::ReloadableMapLayer::ReloadableMapLayer(Text::CString fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, Text::CString layerName) : Map::IMapDrawLayer(fileName, 0, layerName)
{
	this->parsers = parsers;
	this->browser = browser;
	this->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	this->currScale = 10000;
	this->currTime = 0;
}

Map::ReloadableMapLayer::~ReloadableMapLayer()
{
	UOSInt i;
	InnerLayerInfo *innerLayer;
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		innerLayer = this->innerLayers.GetItem(i);
		SDEL_CLASS(innerLayer->innerLayer);
		SDEL_STRING(innerLayer->url);
		SDEL_STRING(innerLayer->layerName);
		MemFree(innerLayer);
	}
}

void Map::ReloadableMapLayer::SetCurrScale(Double scale)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	this->currScale = scale;
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->SetCurrScale(scale);
		}
	}
}

void Map::ReloadableMapLayer::SetCurrTimeTS(Int64 timeStamp)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	this->currTime = timeStamp;
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->SetCurrTimeTS(timeStamp);
		}
	}
}

Int64 Map::ReloadableMapLayer::GetTimeStartTS()
{
	Bool first = true;
	Int64 timeStart = 0;
	Int64 v;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
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
	return timeStart;
}

Int64 Map::ReloadableMapLayer::GetTimeEndTS()
{
	Bool first = true;
	Int64 timeEnd = 0;
	Int64 v;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
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
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = 0;
	j = this->innerLayers.GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
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
	return ret;
}

UOSInt Map::ReloadableMapLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = 0;
	j = this->innerLayers.GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = innerLayer->innerLayer->GetObjectIds(&tmpArr, nameArr, mapRate, rect, keepEmpty);
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
	return ret;
}

UOSInt Map::ReloadableMapLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = 0;
	j = this->innerLayers.GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = innerLayer->innerLayer->GetObjectIdsMapXY(&tmpArr, nameArr, rect, keepEmpty);
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
	return ret;
}

Int64 Map::ReloadableMapLayer::GetObjectIdMax()
{
	UOSInt i;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			currId += maxId + 1;
		}
	}
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
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = 0;
	j = this->innerLayers.GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
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

UInt32 Map::ReloadableMapLayer::GetCodePage()
{
	return 0;
}

Bool Map::ReloadableMapLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	Bool isFirst = true;
	UOSInt i;
	Math::RectAreaDbl minMax;
	Math::RectAreaDbl thisBounds;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			if (isFirst)
			{
				if (innerLayer->innerLayer->GetBounds(&minMax))
				{
					isFirst = false;
				}
			}
			else
			{
				if (innerLayer->innerLayer->GetBounds(&thisBounds))
				{
					minMax.tl = minMax.tl.Min(thisBounds.tl);
					minMax.br = minMax.br.Max(thisBounds.br);
				}
			}
		}
	}
	if (isFirst)
	{
		*bounds = Math::RectAreaDbl(0, 0, 0, 0);
		return false;
	}
	else
	{
		*bounds = minMax;
		return true;
	}
}

void *Map::ReloadableMapLayer::BeginGetObject()
{
	UOSInt i;
	this->innerLayerMut.LockRead();
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->sess = innerLayer->innerLayer->BeginGetObject();
		}
	}
	return this;
}
void Map::ReloadableMapLayer::EndGetObject(void *session)
{
	UOSInt i;
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->EndGetObject(innerLayer->sess);
		}
	}
	this->innerLayerMut.UnlockRead();
}

Map::DrawObjectL *Map::ReloadableMapLayer::GetNewObjectById(void *session, Int64 id)
{
	UOSInt i;
	UOSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Map::DrawObjectL *dobj = 0;
	i = 0;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	j = this->innerLayers.GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			if (id >= currId && id <= currId + maxId)
			{
				dobj = innerLayer->innerLayer->GetNewObjectById(session, id - currId);
				break;
			}
			else
			{
				currId += maxId + 1;
			}
		}
		i++;
	}
	return dobj;
}

Math::Geometry::Vector2D *Map::ReloadableMapLayer::GetNewVectorById(void *session, Int64 id)
{
	UOSInt i;
	UOSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Math::Geometry::Vector2D *vec = 0;
	i = 0;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	j = this->innerLayers.GetCount();
	while (i < j)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			maxId = innerLayer->innerLayer->GetObjectIdMax();
			if (id >= currId && id <= currId + maxId)
			{
				vec = innerLayer->innerLayer->GetNewVectorById(session, id - currId);
				break;
			}
			else
			{
				currId += maxId + 1;
			}
		}
		i++;
	}
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
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer;
		innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			csys = innerLayer->innerLayer->GetCoordinateSystem();
			if (csys)
				break;
		}
	}
	if (csys == 0)
		csys = this->csys;
	return csys;
}

void Map::ReloadableMapLayer::SetCoordinateSystem(Math::CoordinateSystem *csys)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, true);
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		InnerLayerInfo *innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->SetCoordinateSystem(csys->Clone());
		}
	}
	SDEL_CLASS(this->csys);
	this->csys = csys;
}

void Map::ReloadableMapLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	InnerLayerInfo *innerLayer;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, true);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
	i = this->innerLayers.GetCount();
	while (i-- > 0)
	{
		innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer)
		{
			innerLayer->innerLayer->AddUpdatedHandler(hdlr, obj);
		}
	}
}

void Map::ReloadableMapLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	InnerLayerInfo *innerLayer;
	Bool chg = false;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, true);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs.GetItem(i) == hdlr && this->updObjs.GetItem(i) == obj)
		{
			this->updHdlrs.RemoveAt(i);
			this->updObjs.RemoveAt(i);
			chg = true;
		}
	}
	if (chg)
	{
		i = this->innerLayers.GetCount();
		while (i-- > 0)
		{
			innerLayer = this->innerLayers.GetItem(i);
			if (innerLayer->innerLayer)
			{
				innerLayer->innerLayer->RemoveUpdatedHandler(hdlr, obj);
			}
		}
	}
}

void Map::ReloadableMapLayer::AddInnerLayer(Text::CString name, Text::CString url, Int32 seconds)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->GetSourceNameObj());
	sb.AllocLeng(url.leng);
	sb.SetEndPtr(Text::URLString::AppendURLPath(sb.ToString(), sb.GetEndPtr(), url));
	InnerLayerInfo *innerLayer;
	innerLayer = MemAlloc(InnerLayerInfo, 1);
	innerLayer->innerLayer = 0;
	innerLayer->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	innerLayer->url = Text::String::New(sb.ToCString());
	if (name.v)
	{
		innerLayer->layerName = Text::String::New(name);
	}
	else
	{
		innerLayer->layerName = 0;
	}
	innerLayer->reloadInterval = seconds;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, true);
	this->innerLayers.Add(innerLayer);
	mutUsage.EndUse();
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
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	if (this->url)
	{
		Text::StrDelNew(this->url);
	}
	this->url = Text::StrCopyNew(url);
	mutUsage.EndUse();
	this->Reload();
}

void Map::ReloadableMapLayer::SetReloadInterval(Int32 seconds)
{
	this->reloadInterval = seconds;
}*/

void Map::ReloadableMapLayer::Reload()
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->innerLayerMut, false);
	i = this->innerLayers.GetCount();
	mutUsage.EndUse();
	while (i-- > 0)
	{
        IO::IStreamData *data = 0;
		InnerLayerInfo *innerLayer;
		mutUsage.BeginUse(false);
		innerLayer = this->innerLayers.GetItem(i);
		if (innerLayer->innerLayer == 0 || innerLayer->reloadInterval != 0)
		{
			data = this->browser->GetData(innerLayer->url->ToCString(), innerLayer->reloadInterval != 0, 0);
		}
		mutUsage.EndUse();
		if (data)
		{
			while (data->IsLoading())
			{
				Sync::Thread::Sleep(10);
			}
			IO::ParserType pt;
			IO::ParsedObject *pobj = this->parsers->ParseFile(data, &pt);
			DEL_CLASS(data);
			if (pobj)
			{
				if (pt == IO::ParserType::MapLayer)
				{
					UOSInt j;
					mutUsage.BeginUse(true);
					SDEL_CLASS(innerLayer->innerLayer);
					innerLayer->innerLayer = (Map::IMapDrawLayer*)pobj;
					innerLayer->innerLayerType = innerLayer->innerLayer->GetLayerType();
					innerLayer->innerLayer->AddUpdatedHandler(InnerUpdated, this);
					j = this->updHdlrs.GetCount();
					while (j-- > 0)
					{
						innerLayer->innerLayer->AddUpdatedHandler(this->updHdlrs.GetItem(j), this->updObjs.GetItem(j));
					}
					mutUsage.EndUse();
					this->innerLayerMut.UnlockWrite();
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

					mutUsage.BeginUse(false);
					j = this->updHdlrs.GetCount();
					while (j-- > 0)
					{
						this->updHdlrs.GetItem(j)(this->updObjs.GetItem(j));
					}
					mutUsage.EndUse();
				}
				else
				{
					DEL_CLASS(pobj);
				}
			}
		}

	}
}
