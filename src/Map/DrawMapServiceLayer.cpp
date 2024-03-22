#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/DrawMapServiceLayer.h"
#include "Map/ScaledMapView.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/VectorImage.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Map::DrawMapServiceLayer::TaskThread(AnyType userObj)
{
	Math::RectAreaDbl bounds;
	Media::ImageList *imgList;
	Math::Size2DDbl size;
	Double dpi;
	NotNullPtr<Map::DrawMapServiceLayer> me = userObj.GetNN<Map::DrawMapServiceLayer>();
	Int64 thisId = 0;
	{
		Text::StringBuilderUTF8 sb;
		Sync::Event evt;
		me->threadEvt = &evt;
		me->threadRunning = true;
		while (!me->threadToStop)
		{
			while (thisId != me->dispId)
			{
				Sync::MutexUsage mutUsage(me->dispMut);
				thisId = me->dispId;
				bounds = me->dispBounds;
				size = me->dispSize;
				dpi = me->dispDPI;
				mutUsage.EndUse();
				sb.ClearStr();
				imgList = me->mapService->DrawMap(bounds, (UInt32)Double2Int32(size.x), (UInt32)Double2Int32(size.y), dpi, &sb);
				if (imgList)
				{
					mutUsage.BeginUse();
					if (me->dispId == thisId)
					{
						NEW_CLASS(me->dispImage, Media::SharedImage(imgList, false));
						me->dispImageURL = Text::String::New(sb.ToCString()).Ptr();
						mutUsage.ReplaceMutex(me->updMut);
						UOSInt i = me->updHdlrs.GetCount();
						while (i-- > 0)
						{
							Data::CallbackStorage<Map::MapRenderer::UpdatedHandler> cb = me->updHdlrs.GetItem(i);
							cb.func(cb.userObj);
						}
					}
					else
					{
						SDEL_CLASS(me->lastImage);
						SDEL_STRING(me->lastImageURL);
						me->lastId = thisId;
						me->lastBounds = bounds;
						me->lastSize = size;
						me->lastDPI = dpi;
						NEW_CLASS(me->lastImage, Media::SharedImage(imgList, false));
						me->lastImageURL = Text::String::New(sb.ToCString()).Ptr();
						mutUsage.ReplaceMutex(me->updMut);
						UOSInt i = me->updHdlrs.GetCount();
						while (i-- > 0)
						{
							Data::CallbackStorage<Map::MapRenderer::UpdatedHandler> cb = me->updHdlrs.GetItem(i);
							cb.func(cb.userObj);
						}
					}
				}
			}
			me->threadEvt->Wait(1000);
		}
	}
	me->threadRunning = false;
	return 0;
}

void Map::DrawMapServiceLayer::ClearDisp()
{
	if (this->dispImage)
	{
		SDEL_CLASS(this->lastImage);
		SDEL_STRING(this->lastImageURL);
		this->lastBounds = this->dispBounds;
		this->lastSize = this->dispSize;
		this->lastDPI = this->dispDPI;
		this->lastId = this->dispId;
		this->lastImage = this->dispImage;
		this->lastImageURL = this->dispImageURL;
		this->dispImage = 0;
		this->dispImageURL = 0;
	}
}

Map::DrawMapServiceLayer::DrawMapServiceLayer(Map::DrawMapService *mapService) : Map::MapDrawLayer(mapService->GetName(), 0, 0, mapService->GetCoordinateSystem()->Clone())
{
	this->mapService = mapService;
	this->dispBounds = mapService->GetInitBounds();
	this->dispSize = Math::Size2DDbl(640, 480);
	this->dispDPI = 96.0;
	this->dispId = 0;
	this->dispImage = 0;
	this->dispImageURL = 0;
	this->lastBounds = this->dispBounds;
	this->lastSize = this->dispSize;
	this->lastDPI = this->dispDPI;
	this->lastId = 0;
	this->lastImage = 0;
	this->lastImageURL = 0;

	this->threadRunning = false;
	this->threadToStop = false;
	Sync::ThreadUtil::Create(TaskThread, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

Map::DrawMapServiceLayer::~DrawMapServiceLayer()
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Clear();
	mutUsage.EndUse();

	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
	this->ClearDisp();
	SDEL_CLASS(this->lastImage);
	SDEL_STRING(this->lastImageURL);
	DEL_CLASS(this->mapService);
}

void Map::DrawMapServiceLayer::SetCurrScale(Double scale)
{
}

NotNullPtr<Map::MapView> Map::DrawMapServiceLayer::CreateMapView(Math::Size2DDbl scnSize)
{
	NotNullPtr<Map::MapView> view;
	NEW_CLASSNN(view, Map::ScaledMapView(this->dispSize, this->dispBounds.GetCenter(), Map::ScaledMapView::CalcScale(this->dispBounds, this->dispSize, this->dispDPI, this->csys->IsProjected()), this->csys->IsProjected()));
	return view;
}

Map::DrawLayerType Map::DrawMapServiceLayer::GetLayerType() const
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::DrawMapServiceLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	return 0;
}

UOSInt Map::DrawMapServiceLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return this->GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::DrawMapServiceLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Sync::MutexUsage mutUsage(this->dispMut);
	if (this->dispBounds == rect)
	{
		if (this->dispImage)
		{
			outArr->Add(this->dispId);
			return 1;
		}
		else if (this->lastImage)
		{
			outArr->Add(this->lastId);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		this->ClearDisp();
		this->dispBounds = rect;
		this->dispId++;
		if (this->lastImage)
		{
			outArr->Add(this->lastId);
			return 1;
		}
		return 0;
	}
}

Int64 Map::DrawMapServiceLayer::GetObjectIdMax() const
{
	return 0;
}

void Map::DrawMapServiceLayer::ReleaseNameArr(NameArray *nameArr)
{
}

Bool Map::DrawMapServiceLayer::GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	switch (strIndex)
	{
	case 0:
		sb->AppendI64(id);
		return true;
	}
	return false;
}

UOSInt Map::DrawMapServiceLayer::GetColumnCnt() const
{
	return 1;
}

UTF8Char *Map::DrawMapServiceLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	switch (colIndex)
	{
	case 0:
		return Text::StrConcatC(buff, UTF8STRC("id"));
	}
	return 0;
}

DB::DBUtil::ColType Map::DrawMapServiceLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	switch (colIndex)
	{
	case 0:
		colSize.Set(20);
		return DB::DBUtil::CT_Int64;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::DrawMapServiceLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt colSize;
	switch (colIndex)
	{
	case 0:
		colDef->SetPK(true);
		sptr = this->GetColumnName(sbuff, colIndex);
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

UInt32 Map::DrawMapServiceLayer::GetCodePage() const
{
	return 0;
}

Bool Map::DrawMapServiceLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	return this->mapService->GetBounds(bounds);
}

void Map::DrawMapServiceLayer::SetDispSize(Math::Size2DDbl size, Double dpi)
{
	Sync::MutexUsage mutUsage(this->dispMut);
	if (this->dispSize != size || this->dispDPI != dpi)
	{
		this->ClearDisp();
		this->dispSize = size;
		this->dispDPI = dpi;
		this->dispId++;
	}	
}

Map::GetObjectSess *Map::DrawMapServiceLayer::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::DrawMapServiceLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::DrawMapServiceLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	Sync::MutexUsage mutUsage(this->dispMut);
	if (this->dispId == id && this->dispImage)
	{
		Math::Geometry::Vector2D *vec;
		NEW_CLASS(vec, Math::Geometry::VectorImage(this->csys->GetSRID(), this->dispImage, this->dispBounds.min, this->dispBounds.max, false, this->dispImageURL, 0, 0));
		return vec;
	}
	else if (this->lastId == id && this->lastImage)
	{
		Math::Geometry::Vector2D *vec;
		NEW_CLASS(vec, Math::Geometry::VectorImage(this->csys->GetSRID(), this->lastImage, this->lastBounds.min, this->lastBounds.max, false, this->lastImageURL, 0, 0));
		return vec;
	}
	return 0;
}

Map::MapDrawLayer::ObjectClass Map::DrawMapServiceLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_DRAW_MAP_SERVICE_LAYER;
}

Bool Map::DrawMapServiceLayer::CanQuery()
{
	return this->mapService->CanQuery();
}

Bool Map::DrawMapServiceLayer::QueryInfos(Math::Coord2DDbl coord, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList)
{
	return this->mapService->QueryInfos(coord, this->dispBounds, (UInt32)Double2Int32(this->dispSize.x), (UInt32)Double2Int32(this->dispSize.y), this->dispDPI, vecList, valueOfstList, nameList, valueList);
}

void Map::DrawMapServiceLayer::AddUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, AnyType obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Add({hdlr, obj});
}

void Map::DrawMapServiceLayer::RemoveUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, AnyType obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<Map::MapRenderer::UpdatedHandler> cb = this->updHdlrs.GetItem(i);
		if (cb.func == hdlr && cb.userObj == obj)
		{
			this->updHdlrs.RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

Map::DrawMapService *Map::DrawMapServiceLayer::GetDrawMapService()
{
	return this->mapService;
}
