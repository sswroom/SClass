#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "DB/DBReader.h"
#include "IO/DirectoryPackage.h"
#include "Map/HKSpeedLimit.h"
#include "Math/CoordinateSystemManager.h"
#include "Parser/ObjParser/FileGDB2Parser.h"

#define LATLONMUL 40
#include <stdio.h>

void Map::HKSpeedLimit::FreeRoute(RouteInfo *route)
{
	SDEL_CLASS(route->vec);
	DEL_CLASS(route);
}

void Map::HKSpeedLimit::FreeIndex()
{
	Data::ArrayList<Int32> *index;
	const Data::ArrayList<Data::ArrayList<Int32>*> *indexList = this->indexMap.GetValues();
	UOSInt i = indexList->GetCount();
	while (i-- > 0)
	{
		index = indexList->GetItem(i);
		DEL_CLASS(index);
	}
	this->indexMap.Clear();
}

void Map::HKSpeedLimit::BuildIndex()
{
	const Data::ArrayList<RouteInfo*> *routeList = this->routeMap.GetValues();
	RouteInfo *route;
	UOSInt i;
	Data::ArrayList<Int32> *index;
	Int32 x;
	Int32 y;
	Int32 minX;
	Int32 maxX;
	Int32 minY;
	Int32 maxY;
	Int64 key;
	this->FreeIndex();
	i = routeList->GetCount();
	if (this->bounds.br.x > 180 || this->bounds.br.y > 90 || this->bounds.tl.x < -180 || this->bounds.tl.y < -90)
	{
		while (i-- > 0)
		{
			route = routeList->GetItem(i);
			minX = (Int32)(route->bounds.tl.x / 5000.0);
			minY = (Int32)(route->bounds.tl.y / 5000.0);
			maxX = (Int32)(route->bounds.br.x / 5000.0);
			maxY = (Int32)(route->bounds.br.y / 5000.0);
			x = minX;
			while (x <= maxX)
			{
				y = minY;
				while (y <= maxY)
				{
					key = (((Int64)x) << 32) | (UInt32)y;
					index = this->indexMap.Get(key);
					if (index == 0)
					{
						NEW_CLASS(index, Data::ArrayList<Int32>());
						this->indexMap.Put(key, index);
					}
					index->Add(route->routeId);
					y++;
				}
				x++;
			}
		}
	}
	else
	{
		while (i-- > 0)
		{
			route = routeList->GetItem(i);
			minX = (Int32)(route->bounds.tl.x * LATLONMUL);
			minY = (Int32)(route->bounds.tl.y * LATLONMUL);
			maxX = (Int32)(route->bounds.br.x * LATLONMUL);
			maxY = (Int32)(route->bounds.br.y * LATLONMUL);
			x = minX;
			while (x <= maxX)
			{
				y = minY;
				while (y <= maxY)
				{
					key = (((Int64)x) << 32) | (UInt32)y;
					index = this->indexMap.Get(key);
					if (index == 0)
					{
						NEW_CLASS(index, Data::ArrayList<Int32>());
						this->indexMap.Put(key, index);
					}
					index->Add(route->routeId);
					y++;
				}
				x++;
			}
		}
	}
}

void Map::HKSpeedLimit::AppendRouteIds(Data::ArrayList<Int32> *routeList, Int32 x, Int32 y)
{
	Int64 key = (((Int64)x) << 32) | (UInt32)y;
	Data::ArrayList<Int32> *index = this->indexMap.Get(key);
	if (index)
	{
		routeList->AddAll(index);
	}
}

Map::HKSpeedLimit::HKSpeedLimit(Text::CString fgdbPath)
{
	IO::DirectoryPackage pkg(fgdbPath);
	Parser::ObjParser::FileGDB2Parser parser;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	RouteInfo *route;
	UOSInt i;
	this->csys = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(Math::CoordinateSystemManager::PCST_HK80);
	this->fgdb = (DB::ReadingDB*)parser.ParseObject(&pkg, 0, IO::ParserType::ReadingDB);
	if (this->fgdb)
	{
		DB::DBReader *r = fgdb->QueryTableData(CSTR_NULL, CSTR("CENTERLINE"), 0, 0, 0, CSTR_NULL, 0);
		if (r)
		{
			UOSInt objIdCol = INVALID_INDEX;
			UOSInt routeIdCol = INVALID_INDEX;
			UOSInt shapeCol = INVALID_INDEX;
			i = r->ColCount();
			while (i-- > 0)
			{
				sptr = r->GetName(i, sbuff);
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OBJECTID")))
				{
					objIdCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SHAPE")))
				{
					shapeCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ROUTE_ID")))
				{
					routeIdCol = i;
				}
			}
			if (objIdCol != INVALID_INDEX && shapeCol != INVALID_INDEX && routeIdCol != INVALID_INDEX)
			{
				while (r->ReadNext())
				{
					NEW_CLASS(route, RouteInfo());
					route->objectId = r->GetInt32(objIdCol);
					route->routeId = r->GetInt32(routeIdCol);
					route->speedLimit = 50;
					route->vec = r->GetVector(shapeCol);
					route->vec->GetBounds(&route->bounds);
					route = this->routeMap.Put(route->routeId, route);
					if (route)
					{
						FreeRoute(route);
					}
				}
			}
			fgdb->CloseReader(r);
		}
		if (this->routeMap.GetCount() > 0)
		{
			r = fgdb->QueryTableData(CSTR_NULL, CSTR("SPEED_LIMIT"), 0, 0, 0, CSTR_NULL, 0);
			if (r)
			{
				UOSInt routeIdCol = INVALID_INDEX;
				UOSInt spdLimitCol = INVALID_INDEX;
				i = r->ColCount();
				while (i-- > 0)
				{
					sptr = r->GetName(i, sbuff);
					if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ROAD_ROUTE_ID")))
					{
						routeIdCol = i;
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SPEED_LIMIT")))
					{
						spdLimitCol = i;
					}
				}
				if (routeIdCol != INVALID_INDEX && spdLimitCol != INVALID_INDEX)
				{
					while (r->ReadNext())
					{
						route = this->routeMap.Get(r->GetInt32(routeIdCol));
						if (route)
						{
							sptr = r->GetStr(spdLimitCol, sbuff, sizeof(sbuff));
							if (sptr && (sptr - sbuff) > 5)
							{
								sptr[-5] = 0;
								Text::StrToInt32(sbuff, &route->speedLimit);
							}
						}
					}
				}
				fgdb->CloseReader(r);
			}

			const Data::ArrayList<RouteInfo*> *routeList = this->routeMap.GetValues();
			Math::RectAreaDbl bounds;
			i = this->routeMap.GetCount();
			i--;
			route = routeList->GetItem(i);
			bounds = route->bounds;
			while (i-- > 0)
			{
				route = routeList->GetItem(i);
				bounds = bounds.MergeArea(route->bounds);
			}
			this->bounds = bounds;
			this->BuildIndex();
		}
//		printf("Speed Limited loaded, count = %d\r\n", (UInt32)this->routeMap.GetCount());
	}
}

Map::HKSpeedLimit::~HKSpeedLimit()
{
	SDEL_CLASS(this->fgdb);
	DEL_CLASS(this->csys);
	const Data::ArrayList<RouteInfo*> *routeList = this->routeMap.GetValues();
	UOSInt i = routeList->GetCount();
	while (i-- > 0) FreeRoute(routeList->GetItem(i));
	this->FreeIndex();
}

const Map::HKSpeedLimit::RouteInfo *Map::HKSpeedLimit::GetNearestRoute(Math::Coord2DDbl pt)
{
	Data::ArrayList<Int32> routeList;
	UOSInt i;
	RouteInfo *route;
	Int32 minRouteId = -1;
	Double minDist = 1000000000;
	Double thisDist;
	Int32 lastRouteId = -1;
	Int32 routeId;
	Int32 xInd;
	Int32 yInd;
	if (this->bounds.br.x > 180 || this->bounds.br.y > 90 || this->bounds.tl.x < -180 || this->bounds.tl.y < -90)
	{
		xInd = (Int32)(pt.x / 5000.0);
		yInd = (Int32)(pt.y / 5000.0);
	}
	else
	{
		xInd = (Int32)(pt.x * LATLONMUL);
		yInd = (Int32)(pt.y * LATLONMUL);
	}
	AppendRouteIds(&routeList, xInd - 1, yInd - 1);
	AppendRouteIds(&routeList, xInd + 0, yInd - 1);
	AppendRouteIds(&routeList, xInd + 1, yInd - 1);
	AppendRouteIds(&routeList, xInd - 1, yInd + 0);
	AppendRouteIds(&routeList, xInd + 0, yInd + 0);
	AppendRouteIds(&routeList, xInd + 1, yInd + 0);
	AppendRouteIds(&routeList, xInd - 1, yInd + 1);
	AppendRouteIds(&routeList, xInd + 0, yInd + 1);
	AppendRouteIds(&routeList, xInd + 1, yInd + 1);
	ArtificialQuickSort_SortInt32(routeList.GetArray(&i), 0, (OSInt)routeList.GetCount() - 1);
	i = routeList.GetCount();
	while (i-- > 0)
	{
		routeId = routeList.GetItem(i);
		if (routeId != lastRouteId)
		{
			lastRouteId = routeId;
			route = this->routeMap.Get(routeId);
			if (route)
			{
				thisDist = route->vec->CalSqrDistance(pt, 0);
				if (thisDist < minDist)
				{
					minDist = thisDist;
					minRouteId = routeId;
				}
			}
		}
	}

	return this->routeMap.Get(minRouteId);
}

void Map::HKSpeedLimit::ConvertCoordinateSystem(Math::CoordinateSystem *csys)
{
	if (csys && !this->csys->Equals(csys))
	{
		Double tmp;
		RouteInfo *route;
		Math::CoordinateSystem::ConvertXYZ(this->csys, csys, this->bounds.tl.x, this->bounds.tl.y, 0, &this->bounds.tl.x, &this->bounds.tl.y, &tmp);
		Math::CoordinateSystem::ConvertXYZ(this->csys, csys, this->bounds.br.x, this->bounds.br.y, 0, &this->bounds.br.x, &this->bounds.br.y, &tmp);
		const Data::ArrayList<RouteInfo*> *routeList = this->routeMap.GetValues();
		UOSInt i = routeList->GetCount();
		while (i-- > 0)
		{
			route = routeList->GetItem(i);
			route->vec->ConvCSys(this->csys, csys);
			route->vec->GetBounds(&route->bounds);
		}
		DEL_CLASS(this->csys);
		this->csys = csys->Clone();
		this->BuildIndex();
	}
}


Text::CString Map::HKSpeedLimit::GetDownloadURL()
{
	return CSTR("https://static.data.gov.hk/td/road-network-v2/RdNet_IRNP.gdb.zip");
}

Text::CString Map::HKSpeedLimit::GetDefFileName()
{
	return CSTR("RdNet_IRNP.gdb");
}