#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/DrawMapServiceLayer.h"
#include "Map/ScaledMapView.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/VectorImage.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

UInt32 __stdcall Map::DrawMapServiceLayer::TaskThread(void *userObj)
{
	Math::RectAreaDbl bounds;
	Media::ImageList *imgList;
	Math::Size2D<Double> size;
	Double dpi;
	Map::DrawMapServiceLayer *me = (Map::DrawMapServiceLayer*)userObj;
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
				Sync::MutexUsage mutUsage(&me->dispMut);
				thisId = me->dispId;
				bounds = me->dispBounds;
				size = me->dispSize;
				dpi = me->dispDPI;
				mutUsage.EndUse();
				sb.ClearStr();
				imgList = me->mapService->DrawMap(bounds, (UInt32)Double2Int32(size.width), (UInt32)Double2Int32(size.height), dpi, &sb);
				if (imgList)
				{
					mutUsage.BeginUse();
					if (me->dispId == thisId)
					{
						NEW_CLASS(me->dispImage, Media::SharedImage(imgList, false));
						me->dispImageURL = Text::String::New(sb.ToCString());
						mutUsage.ReplaceMutex(&me->updMut);
						UOSInt i = me->updHdlrs.GetCount();
						while (i-- > 0)
						{
							me->updHdlrs.GetItem(i)(me->updObjs.GetItem(i));
						}
					}
					else
					{
						DEL_CLASS(imgList);
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
	SDEL_CLASS(this->dispImage);
	SDEL_STRING(this->dispImageURL);
}

Map::DrawMapServiceLayer::DrawMapServiceLayer(Map::DrawMapService *mapService) : Map::IMapDrawLayer(mapService->GetName(), 0, 0)
{
	this->mapService = mapService;
	this->csys = mapService->GetCoordinateSystem()->Clone();
	this->dispBounds = mapService->GetInitBounds();
	this->dispSize = Math::Size2D<Double>(640, 480);
	this->dispDPI = 96.0;
	this->dispId = 0;
	this->dispImage = 0;
	this->dispImageURL = 0;

	this->threadRunning = false;
	this->threadToStop = false;
	Sync::Thread::Create(TaskThread, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
}

Map::DrawMapServiceLayer::~DrawMapServiceLayer()
{
	Sync::MutexUsage mutUsage(&this->updMut);
	this->updHdlrs.Clear();
	this->updObjs.Clear();
	mutUsage.EndUse();

	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
	this->ClearDisp();
	DEL_CLASS(this->mapService);
}

void Map::DrawMapServiceLayer::SetCurrScale(Double scale)
{
}

Map::MapView *Map::DrawMapServiceLayer::CreateMapView(Math::Size2D<Double> scnSize)
{
	Map::MapView *view;
	NEW_CLASS(view, Map::ScaledMapView(this->dispSize, this->dispBounds.GetCenter(), Map::ScaledMapView::CalcScale(this->dispBounds, this->dispSize, this->dispDPI, this->csys->IsProjected()), this->csys->IsProjected()));
	return view;
}

Map::DrawLayerType Map::DrawMapServiceLayer::GetLayerType()
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::DrawMapServiceLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	return 0;
}

UOSInt Map::DrawMapServiceLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return this->GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() * mapRate, keepEmpty);
}

UOSInt Map::DrawMapServiceLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Sync::MutexUsage mutUsage(&this->dispMut);
	if (this->dispBounds == rect)
	{
		if (this->dispImage)
		{
			outArr->Add(this->dispId);
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
		return 0;
	}
}

Int64 Map::DrawMapServiceLayer::GetObjectIdMax()
{
	return 0;
}

void Map::DrawMapServiceLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::DrawMapServiceLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	switch (strIndex)
	{
	case 0:
		return Text::StrInt64(buff, id);
	}
	return 0;
}

UOSInt Map::DrawMapServiceLayer::GetColumnCnt()
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

DB::DBUtil::ColType Map::DrawMapServiceLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	switch (colIndex)
	{
	case 0:
		if (colSize) *colSize = 20;
		return DB::DBUtil::CT_Int64;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::DrawMapServiceLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
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
		colDef->SetColType(this->GetColumnType(colIndex, &colSize));
		colDef->SetColSize(colSize);
		colDef->SetColDP(0);
		colDef->SetNotNull(true);
		colDef->SetAutoInc(false);
		return true;
	}
	return false;
}

UInt32 Map::DrawMapServiceLayer::GetCodePage()
{
	return 0;
}

Bool Map::DrawMapServiceLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	return this->mapService->GetBounds(bounds);
}

void Map::DrawMapServiceLayer::SetDispSize(Math::Size2D<Double> size, Double dpi)
{
	Sync::MutexUsage mutUsage(&this->dispMut);
	if (this->dispSize != size || this->dispDPI != dpi)
	{
		this->ClearDisp();
		this->dispSize = size;
		this->dispDPI = dpi;
		this->dispId++;
	}	
}

void *Map::DrawMapServiceLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::DrawMapServiceLayer::EndGetObject(void *session)
{
}

Math::Geometry::Vector2D *Map::DrawMapServiceLayer::GetNewVectorById(void *session, Int64 id)
{
	Sync::MutexUsage mutUsage(&this->dispMut);
	if (this->dispId == id && this->dispImage)
	{
		Math::Geometry::Vector2D *vec;
		NEW_CLASS(vec, Math::Geometry::VectorImage(this->csys->GetSRID(), this->dispImage, this->dispBounds.tl, this->dispBounds.br, false, this->dispImageURL, 0, 0));
		return vec;
	}
	return 0;
}

Map::IMapDrawLayer::ObjectClass Map::DrawMapServiceLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_DRAW_MAP_SERVICE_LAYER;
}

Bool Map::DrawMapServiceLayer::CanQuery()
{
	return this->mapService->CanQuery();
}

Math::Geometry::Vector2D *Map::DrawMapServiceLayer::QueryInfo(Math::Coord2DDbl coord, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	return this->mapService->QueryInfo(coord, this->dispBounds, (UInt32)Double2Int32(this->dispSize.width), (UInt32)Double2Int32(this->dispSize.height), this->dispDPI, nameList, valueList);
}

void Map::DrawMapServiceLayer::AddUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(&this->updMut);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
	mutUsage.EndUse();
}

void Map::DrawMapServiceLayer::RemoveUpdatedHandler(Map::MapRenderer::UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(&this->updMut);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs.GetItem(i) == hdlr && this->updObjs.GetItem(i) == obj)
		{
			this->updObjs.RemoveAt(i);
			this->updHdlrs.RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

Map::DrawMapService *Map::DrawMapServiceLayer::GetDrawMapService()
{
	return this->mapService;
}