#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/NetworkLinkLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Sync/MutexUsage.h"
#include "Sync/RWMutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void __stdcall Map::NetworkLinkLayer::InnerUpdated(AnyType userObj)
{
	NotNullPtr<Map::NetworkLinkLayer> me = userObj.GetNN<Map::NetworkLinkLayer>();
	UOSInt i;
	Sync::RWMutexUsage mutUsage(me->linkMut, false);
	i = me->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = me->updHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

UInt32 __stdcall Map::NetworkLinkLayer::ControlThread(void *userObj)
{
	Map::NetworkLinkLayer *me = (Map::NetworkLinkLayer*)userObj;
	me->ctrlRunning = true;
	while (!me->ctrlToStop)
	{
		me->CheckLinks(false);
		me->ctrlEvt.Wait(1000);
	}
	me->ctrlRunning = false;
	return 0;
}

void Map::NetworkLinkLayer::CheckLinks(Bool manualRequest)
{
	UOSInt i;
	Data::Timestamp currTime = Data::Timestamp::UtcNow();
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
	i = this->links.GetCount();
	mutUsage.EndUse();
	while (i-- > 0)
	{
		LinkInfo *link;
		mutUsage.BeginUse(false);
		link = this->links.GetItem(i);
		mutUsage.EndUse();

		if (link->lastUpdated.IsNull())
		{
			this->LoadLink(link);
		}
		else
		{
			switch (link->mode)
			{
			case RefreshMode::OnRequest:
				if (manualRequest)
				{
					this->LoadLink(link);
				}
				break;
			case RefreshMode::OnInterval:
				if (link->reloadInterval != 0 && currTime.DiffMS(link->lastUpdated) >= link->reloadInterval * 1000)
				{
					this->LoadLink(link);
				}
				break;
			case RefreshMode::OnStop:
				if (this->dispTime > link->lastUpdated.ToTicks() && (currTime.ToTicks() - this->dispTime) >= link->reloadInterval * 1000)
				{
					this->LoadLink(link);
				}
				break;
			}
		}
	}
}

void Map::NetworkLinkLayer::LoadLink(LinkInfo *link)
{
	IO::StreamData *data = 0;
	if (link->viewFormat.IsNull())
	{
#if defined(VERBOSE)
		printf("NetworkLnkLayer: Loading URL: %s\r\n", link->url->v);
#endif
		data = this->browser->GetData(link->url->ToCString(), true, 0);
	}
	else
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		Text::StringBuilderUTF8 sb;
		Double width = this->dispSize.GetWidth();
		Double height = this->dispSize.GetHeight();
		if (this->dispDPI > 96.0)
		{
			width = width * 96.0 / this->dispDPI;
			height = height * 96.0 / this->dispDPI;
		}
		sb.Append(link->url);
		sb.AppendUTF8Char('?');
		sb.AppendOpt(link->viewFormat);
		Sync::MutexUsage dispMutUsage(this->dispMut);
		sptr = Text::StrDouble(sbuff, this->dispRect.min.x);
		sb.ReplaceStr(UTF8STRC("[bboxWest]"), sbuff, (UOSInt)(sptr - sbuff));
		sptr = Text::StrDouble(sbuff, this->dispRect.min.y);
		sb.ReplaceStr(UTF8STRC("[bboxSouth]"), sbuff, (UOSInt)(sptr - sbuff));
		sptr = Text::StrDouble(sbuff, this->dispRect.max.x);
		sb.ReplaceStr(UTF8STRC("[bboxEast]"), sbuff, (UOSInt)(sptr - sbuff));
		sptr = Text::StrDouble(sbuff, this->dispRect.max.y);
		sb.ReplaceStr(UTF8STRC("[bboxNorth]"), sbuff, (UOSInt)(sptr - sbuff));
		sptr = Text::StrInt32(sbuff, Double2Int32(width));
		sb.ReplaceStr(UTF8STRC("[horizPixels]"), sbuff, (UOSInt)(sptr - sbuff));
		sptr = Text::StrInt32(sbuff, Double2Int32(height));
		sb.ReplaceStr(UTF8STRC("[vertPixels]"), sbuff, (UOSInt)(sptr - sbuff));
#if defined(VERBOSE)
		printf("NetworkLnkLayer: Loading URL: %s\r\n", sb.ToString());
#endif
		data = this->browser->GetData(sb.ToCString(), true, 0);
	}
	NotNullPtr<IO::StreamData> fd;
	if (fd.Set(data))
	{
		while (fd->IsLoading())
		{
			Sync::SimpleThread::Sleep(10);
		}
#if defined(VERBOSE)
		printf("NetworkLnkLayer: Data size: %lld\r\n", fd->GetDataSize());
#endif
		link->lastUpdated = Data::Timestamp::UtcNow();
		IO::ParsedObject *pobj = this->parsers->ParseFileType(fd, IO::ParserType::MapLayer);
		fd.Delete();
		if (pobj)
		{
			UOSInt j;
			Sync::RWMutexUsage mutUsage(this->linkMut, true);
			SDEL_CLASS(link->innerLayer);
			link->innerLayer = (Map::MapDrawLayer*)pobj;
			link->innerLayerType = link->innerLayer->GetLayerType();
			link->innerLayer->AddUpdatedHandler(InnerUpdated, this);
			j = this->updHdlrs.GetCount();
			while (j-- > 0)
			{
				Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = this->updHdlrs.GetItem(j);
				link->innerLayer->AddUpdatedHandler(cb.func, cb.userObj);
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
				Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = this->updHdlrs.GetItem(j);
				cb.func(cb.userObj);
			}
			mutUsage.EndUse();
		}
	}
	else
	{
		link->lastUpdated = Data::Timestamp::UtcNow();
	}
}

Map::NetworkLinkLayer::NetworkLinkLayer(Text::CStringNN fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, Text::CString layerName) : Map::MapDrawLayer(fileName, 0, layerName, Math::CoordinateSystemManager::CreateDefaultCsys())
{
	this->parsers = parsers;
	this->browser = browser;
	this->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	this->currScale = 10000;
	this->currTime = 0;
	this->dispSize = Math::Size2DDbl(640, 480);
	this->dispDPI = 96.0;
	this->dispRect = Math::RectAreaDbl(0, 0, 0, 0);
	this->dispTime = Data::DateTimeUtil::GetCurrTimeMillis();
	this->hasBounds = false;

	this->ctrlRunning = false;
	this->ctrlToStop = false;
	Sync::ThreadUtil::Create(ControlThread, this);
	while (!this->ctrlRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Map::NetworkLinkLayer::~NetworkLinkLayer()
{
	this->ctrlToStop = true;
	this->ctrlEvt.Set();
	while (this->ctrlRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	UOSInt i;
	LinkInfo *link;
	i = this->links.GetCount();
	while (i-- > 0)
	{
		link = this->links.GetItem(i);
		SDEL_CLASS(link->innerLayer);
		link->url->Release();
		OPTSTR_DEL(link->viewFormat);
		OPTSTR_DEL(link->layerName);
		DEL_CLASS(link);
	}
}

void Map::NetworkLinkLayer::SetCurrScale(Double scale)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
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
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
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

Int64 Map::NetworkLinkLayer::GetTimeStartTS() const
{
	Bool first = true;
	Int64 timeStart = 0;
	Int64 v;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
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

Int64 Map::NetworkLinkLayer::GetTimeEndTS() const
{
	Bool first = true;
	Int64 timeEnd = 0;
	Int64 v;
	UOSInt i;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
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

Map::DrawLayerType Map::NetworkLinkLayer::GetLayerType() const
{
	if (this->innerLayerType == Map::DRAW_LAYER_UNKNOWN)
		return Map::DRAW_LAYER_MIXED;
	return this->innerLayerType;
}

UOSInt Map::NetworkLinkLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
	i = 0;
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = link->innerLayer->GetAllObjectIds(tmpArr, nameArr);
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

UOSInt Map::NetworkLinkLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	{
		Sync::MutexUsage mutUsage(this->dispMut);
		Math::RectAreaDbl newRect = rect.ToDouble() / mapRate;
		if (this->dispRect != newRect)
		{
			this->dispRect = newRect;
			this->dispTime = Data::DateTimeUtil::GetCurrTimeMillis();
#if defined(VERBOSE)
			printf("NetworkLnkLayer: Update dispRect\r\n");
#endif
		}
	}
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
	i = 0;
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = link->innerLayer->GetObjectIds(tmpArr, nameArr, mapRate, rect, keepEmpty);
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

UOSInt Map::NetworkLinkLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	{
		Sync::MutexUsage mutUsage(this->dispMut);
		if (this->dispRect != rect)
		{
			this->dispRect = rect;
			this->dispTime = Data::DateTimeUtil::GetCurrTimeMillis();
#if defined(VERBOSE)
			printf("NetworkLnkLayer: Update dispRect\r\n");
#endif
		}
	}
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId = 0;
	Int64 maxId;
	UOSInt ret = 0;
	Data::ArrayListInt64 tmpArr;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
	i = 0;
	j = this->links.GetCount();
	while (i < j)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			maxId = link->innerLayer->GetObjectIdMax();
			tmpArr.Clear();
			l = link->innerLayer->GetObjectIdsMapXY(tmpArr, nameArr, rect, keepEmpty);
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

Int64 Map::NetworkLinkLayer::GetObjectIdMax() const
{
	UOSInt i;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
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
void Map::NetworkLinkLayer::ReleaseNameArr(NameArray *nameArr)
{
/*	Map::MapDrawLayer *lyr = this->innerLayer;
	if (lyr)
		lyr->ReleaseNameArr(nameArr);*/
}

Bool Map::NetworkLinkLayer::GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	UOSInt i;
	UOSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
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
				return link->innerLayer->GetString(sb, 0, id - currId, strIndex);
			}
			currId += maxId + 1;
		}
		i++;
	}
	return false;
}

UOSInt Map::NetworkLinkLayer::GetColumnCnt() const
{
	return 0;
}

UTF8Char *Map::NetworkLinkLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType Map::NetworkLinkLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::NetworkLinkLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	return false;
}

UInt32 Map::NetworkLinkLayer::GetCodePage() const
{
	return 0;
}

Bool Map::NetworkLinkLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	if (this->hasBounds)
	{
		bounds.Set(this->bounds);
		return true;
	}
	Bool isFirst = true;
	UOSInt i;
	Math::RectAreaDbl minMax;
	Math::RectAreaDbl thisBounds;
	Sync::RWMutexUsage mutUsage(NotNullPtr<Sync::RWMutex>::ConvertFrom(NotNullPtr<const Sync::RWMutex>(this->linkMut)), false);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			if (isFirst)
			{
				if (link->innerLayer->GetBounds(minMax))
				{
					isFirst = false;
				}
			}
			else
			{
				if (link->innerLayer->GetBounds(thisBounds))
				{
					minMax = minMax.MergeArea(thisBounds);
				}
			}
		}
	}
	if (isFirst)
	{
		bounds.Set(Math::RectAreaDbl(0, 0, 0, 0));
		return false;
	}
	else
	{
		bounds.Set(minMax);
		return true;
	}
}

void Map::NetworkLinkLayer::SetDispSize(Math::Size2DDbl size, Double dpi)
{
	Sync::MutexUsage mutUsage(this->dispMut);
	this->dispSize = size;
	this->dispDPI = dpi;
#if defined(VERBOSE)
	printf("NetworkLnkLayer: Update dispSize\r\n");
#endif
}

Map::GetObjectSess *Map::NetworkLinkLayer::BeginGetObject()
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
	return (Map::GetObjectSess*)this;
}
void Map::NetworkLinkLayer::EndGetObject(GetObjectSess *session)
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

Math::Geometry::Vector2D *Map::NetworkLinkLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	UOSInt i;
	UOSInt j;
	Int64 currId = 0;
	Int64 maxId;
	Math::Geometry::Vector2D *vec = 0;
	i = 0;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
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

UOSInt Map::NetworkLinkLayer::GetNameCol() const
{
	return 0;
}
void Map::NetworkLinkLayer::SetNameCol(UOSInt nameCol)
{
}

Map::MapDrawLayer::ObjectClass Map::NetworkLinkLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_NETWORKLINK_LAYER;
}

NotNullPtr<Math::CoordinateSystem> Map::NetworkLinkLayer::GetCoordinateSystem()
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(this->linkMut, false);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link;
		link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			return link->innerLayer->GetCoordinateSystem();
		}
	}
	return this->csys;
}

void Map::NetworkLinkLayer::SetCoordinateSystem(NotNullPtr<Math::CoordinateSystem> csys)
{
	UOSInt i;
	Sync::RWMutexUsage mutUsage(this->linkMut, true);
	i = this->links.GetCount();
	while (i-- > 0)
	{
		LinkInfo *link = this->links.GetItem(i);
		if (link->innerLayer)
		{
			link->innerLayer->SetCoordinateSystem(csys->Clone());
		}
	}
	this->csys.Delete();;
	this->csys = csys;
}

void Map::NetworkLinkLayer::AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
	UOSInt i;
	LinkInfo *link;
	Sync::RWMutexUsage mutUsage(this->linkMut, true);
	this->updHdlrs.Add({hdlr, obj});
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

void Map::NetworkLinkLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
	UOSInt i;
	LinkInfo *link;
	Bool chg = false;
	Sync::RWMutexUsage mutUsage(this->linkMut, true);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler> cb = this->updHdlrs.GetItem(i);
		if (cb.func == hdlr && cb.userObj == obj)
		{
			this->updHdlrs.RemoveAt(i);
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

UOSInt Map::NetworkLinkLayer::AddLink(Text::CString name, Text::CStringNN url, Text::CString viewFormat, RefreshMode mode, Int32 seconds)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->GetSourceNameObj());
	sb.AllocLeng(url.leng);
	sb.SetEndPtr(Text::URLString::AppendURLPath(sb.v, sb.GetEndPtr(), url));
	LinkInfo *link;
	NEW_CLASS(link, LinkInfo());
	link->innerLayer = 0;
	link->innerLayerType = Map::DRAW_LAYER_UNKNOWN;
	link->url = Text::String::New(sb.ToCString());
	link->layerName = Text::String::NewOrNull(name);
	link->viewFormat = Text::String::NewOrNull(viewFormat);
	link->mode = mode;
	link->reloadInterval = seconds;
	link->lastUpdated = Data::Timestamp(0);
	if (mode == RefreshMode::OnStop)
	{
		link->lastUpdated = Data::Timestamp::UtcNow();
	}
	Sync::RWMutexUsage mutUsage(this->linkMut, true);
	UOSInt ret = this->links.Add(link);
	mutUsage.EndUse();
	this->ctrlEvt.Set();
	return ret;
}

void Map::NetworkLinkLayer::SetBounds(Math::RectAreaDbl bounds)
{
	this->bounds = bounds;
	this->hasBounds = true;
}

void Map::NetworkLinkLayer::Reload()
{
	this->CheckLinks(true);
}
