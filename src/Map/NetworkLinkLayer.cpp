#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/NetworkLinkLayer.h"
#include "Sync/MutexUsage.h"
#include "Sync/RWMutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"

void __stdcall Map::NetworkLinkLayer::InnerUpdated(void *userObj)
{
	Map::NetworkLinkLayer *me = (Map::NetworkLinkLayer*)userObj;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&me->linkMut, false);
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		me->updHdlrs.GetItem(i)(me->updObjs.GetItem(i));
	}
}

void Map::NetworkLinkLayer::LoadLink(LinkInfo *link)
{
	IO::IStreamData *data = 0;
	if (link->innerLayer == 0 || link->reloadInterval != 0)
	{
		data = this->browser->GetData(link->url->ToCString(), link->reloadInterval != 0, 0);
	}
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
				Sync::RWMutexUsage mutUsage(&this->linkMut, true);
				SDEL_CLASS(link->innerLayer);
				link->innerLayer = (Map::IMapDrawLayer*)pobj;
				link->innerLayerType = link->innerLayer->GetLayerType();
				link->innerLayer->AddUpdatedHandler(InnerUpdated, this);
				j = this->updHdlrs.GetCount();
				while (j-- > 0)
				{
					link->innerLayer->AddUpdatedHandler(this->updHdlrs.GetItem(j), this->updObjs.GetItem(j));
				}
				mutUsage.EndUse();
				if (this->innerLayerType != link->innerLayerType)
				{
					if (this->innerLayerType == Map::DRAW_LAYER_UNKNOWN)
					{
						this->innerLayerType = link->innerLayerType;
					}
					else if (link->innerLayerType == Map::DRAW_LAYER_UNKNOWN || link->innerLayerType == this->innerLayerType)
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

Map::NetworkLinkLayer::NetworkLinkLayer(Text::CString fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, Text::CString layerName) : Map::IMapDrawLayer(fileName, 0, layerName)
{
	this->parsers = parsers;
	this->browser = browser;
	this->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	this->currScale = 10000;
	this->currTime = 0;
	this->dispSize = Math::Size2D<Double>(640, 480);
	this->dispDPI = 96.0;
	this->dispRect = Math::RectAreaDbl(0, 0, 0, 0);
}

Map::NetworkLinkLayer::~NetworkLinkLayer()
{
	UOSInt i;
	LinkInfo *link;
	i = this->links.GetCount();
	while (i-- > 0)
	{
		link = this->links.GetItem(i);
		SDEL_CLASS(link->innerLayer);
		SDEL_STRING(link->url);
		SDEL_STRING(link->viewFormat);
		SDEL_STRING(link->layerName);
		MemFree(link);
	}
}

void Map::NetworkLinkLayer::SetCurrScale(Double scale)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	this->currScale = scale;
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			link->innerLayer->SetCurrScale(scale);
		}
	}
}

void Map::NetworkLinkLayer::SetCurrTimeTS(Int64 timeStamp)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	this->currTime = timeStamp;
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			link->innerLayer->SetCurrTimeTS(timeStamp);
		}
	}
}

Int64 Map::NetworkLinkLayer::GetTimeStartTS()
{
	Bool first = true;
	Int64 timeStart = 0;
	Int64 v;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			if (first)
			{
				timeStart = link->innerLayer->GetTimeStartTS();
				first = (timeStart == 0);
			}
			else
			{
				v = link->innerLayer->GetTimeStartTS();
				if (v != 0 && v < timeStart)
				{
					timeStart = v;
				}
			}
		}
	}
	return timeStart;
}

Int64 Map::NetworkLinkLayer::GetTimeEndTS()
{
	Bool first = true;
	Int64 timeEnd = 0;
	Int64 v;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			if (first)
			{
				timeEnd = link->innerLayer->GetTimeEndTS();
				first = (timeEnd == 0);
			}
			else
			{
				v = link->innerLayer->GetTimeEndTS();
				if (v != 0 && v > timeEnd)
				{
					timeEnd = v;
				}
			}
		}
	}
	return timeEnd;
}

Map::DrawLayerType Map::NetworkLinkLayer::GetLayerType()
{
	return this->innerLayerType;
}

UOSInt Map::NetworkLinkLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = 0;
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = link->innerLayer->GetAllObjectIds(&tmpArr, nameArr);
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

UOSInt Map::NetworkLinkLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	{
		Sync::MutexUsage mutUsage(&this->dispMut);
		this->dispRect = rect.ToDouble() / mapRate;	
	}
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = 0;
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = link->innerLayer->GetObjectIds(&tmpArr, nameArr, mapRate, rect, keepEmpty);
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

UOSInt Map::NetworkLinkLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	{
		Sync::MutexUsage mutUsage(&this->dispMut);
		this->dispRect = rect;	
	}
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = 0;
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = link->innerLayer->GetObjectIdsMapXY(&tmpArr, nameArr, rect, keepEmpty);
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

Int64 Map::NetworkLinkLayer::GetObjectIdMax()
{
	UOSInt i;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			currId += maxId + 1;
		}
	}
	return currId - 1;
}
void Map::NetworkLinkLayer::ReleaseNameArr(void *nameArr)
{
/*	Map::IMapDrawLayer *lyr = this->innerLayer;
	if (lyr)
		lyr->ReleaseNameArr(nameArr);*/
}

UTF8Char *Map::NetworkLinkLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	UOSInt i;
	UOSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = 0;
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			if (id >= currId && id <= currId + maxId)
			{
				return link->innerLayer->GetString(buff, buffSize, 0, id - currId, strIndex);
			}
			currId += maxId + 1;
		}
		i++;
	}
	return 0;
}

UOSInt Map::NetworkLinkLayer::GetColumnCnt()
{
	return 0;
}

UTF8Char *Map::NetworkLinkLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType Map::NetworkLinkLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::NetworkLinkLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return false;
}

UInt32 Map::NetworkLinkLayer::GetCodePage()
{
	return 0;
}

Bool Map::NetworkLinkLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	Bool isFirst = true;
	UOSInt i;
	Math::RectAreaDbl minMax;
	Math::RectAreaDbl thisBounds;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			if (isFirst)
			{
				if (link->innerLayer->GetBounds(&minMax))
				{
					isFirst = false;
				}
			}
			else
			{
				if (link->innerLayer->GetBounds(&thisBounds))
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

void Map::NetworkLinkLayer::SetDispSize(Math::Size2D<Double> size, Double dpi)
{
	Sync::MutexUsage mutUsage(&this->dispMut);
	this->dispSize = size;
	this->dispDPI = dpi;
}

void *Map::NetworkLinkLayer::BeginGetObject()
{
	UOSInt i;
	this->linkMut.LockRead();
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			link->sess = link->innerLayer->BeginGetObject();
		}
	}
	return this;
}
void Map::NetworkLinkLayer::EndGetObject(void *session)
{
	UOSInt i;
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			link->innerLayer->EndGetObject(link->sess);
		}
	}
	this->linkMut.UnlockRead();
}

Math::Geometry::Vector2D *Map::NetworkLinkLayer::GetNewVectorById(void *session, Int64 id)
{
	UOSInt i;
	UOSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Math::Geometry::Vector2D *vec = 0;
	i = 0;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			if (id >= currId && id <= currId + maxId)
			{
				vec = link->innerLayer->GetNewVectorById(session, id - currId);
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

UOSInt Map::NetworkLinkLayer::GetNameCol()
{
	return 0;
}
void Map::NetworkLinkLayer::SetNameCol(UOSInt nameCol)
{
}

Map::IMapDrawLayer::ObjectClass Map::NetworkLinkLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_NETWORKLINK_LAYER;
}

Math::CoordinateSystem *Map::NetworkLinkLayer::GetCoordinateSystem()
{
	Math::CoordinateSystem *csys = 0;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link;
		link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			csys = link->innerLayer->GetCoordinateSystem();
			if (csys)
				break;
		}
	}
	if (csys == 0)
		csys = this->csys;
	return csys;
}

void Map::NetworkLinkLayer::SetCoordinateSystem(Math::CoordinateSystem *csys)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->linkMut, true);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			link->innerLayer->SetCoordinateSystem(csys->Clone());
		}
	}
	SDEL_CLASS(this->csys);
	this->csys = csys;
}

void Map::NetworkLinkLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	LinkInfo *link;
	Sync::RWMutexUsage mutUsage(&this->linkMut, true);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			link->innerLayer->AddUpdatedHandler(hdlr, obj);
		}
	}
}

void Map::NetworkLinkLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	LinkInfo *link;
	Bool chg = false;
	Sync::RWMutexUsage mutUsage(&this->linkMut, true);
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
		i = this->links.GetCount();
		while (i-- > 0)
		{
			link = this->links.GetItem(i);
			if (link->innerLayer)
			{
				link->innerLayer->RemoveUpdatedHandler(hdlr, obj);
			}
		}
	}
}

void Map::NetworkLinkLayer::AddLink(Text::CString name, Text::CString url, Text::CString viewFormat, RefreshMode mode, Int32 seconds)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->GetSourceNameObj());
	sb.AllocLeng(url.leng);
	sb.SetEndPtr(Text::URLString::AppendURLPath(sb.v, sb.GetEndPtr(), url));
	LinkInfo *link;
	link = MemAlloc(LinkInfo, 1);
	link->innerLayer = 0;
	link->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	link->url = Text::String::New(sb.ToCString());
	link->layerName = Text::String::NewOrNull(name);
	link->viewFormat = Text::String::NewOrNull(viewFormat);
	link->mode = mode;
	link->reloadInterval = seconds;
	link->lastUpdated = Data::Timestamp(0, 0);
	Sync::RWMutexUsage mutUsage(&this->linkMut, true);
	this->links.Add(link);
	mutUsage.EndUse();
	this->Reload();
}

void Map::NetworkLinkLayer::Reload()
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(&this->linkMut, false);
	i = this->links.GetCount();
	mutUsage.EndUse();
	while (i-- > 0)
	{
		LinkInfo *link;
		mutUsage.BeginUse(false);
		link = this->links.GetItem(i);
		mutUsage.EndUse();
		this->LoadLink(link);
	}
}
