#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "DB/DBReader.h"
#include "Map/HKSpeedLimit.h"

#define PROJECTMuL 1000.0
#define LATLONMUL 40
#include <stdio.h>

void Map::HKSpeedLimit::FreeRoute(RouteInfo *route)
{
	SDEL_CLASS(route->vecOri);
	DEL_CLASS(route);
}

void Map::HKSpeedLimit::FreeIndex()
{
	Data::ArrayList<Int32> *index;
	UOSInt i = this->indexMap.GetCount();
	while (i-- > 0)
	{
		index = this->indexMap.GetItem(i);
		DEL_CLASS(index);
	}
	this->indexMap.Clear();
}

void Map::HKSpeedLimit::BuildIndex()
{
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
	i = this->routeMap.GetCount();
	if (this->bounds.br.x > 180 || this->bounds.br.y > 90 || this->bounds.tl.x < -180 || this->bounds.tl.y < -90)
	{
		while (i-- > 0)
		{
			route = this->routeMap.GetItem(i);
			minX = (Int32)(route->bounds.tl.x / PROJECTMuL);
			minY = (Int32)(route->bounds.tl.y / PROJECTMuL);
			maxX = (Int32)(route->bounds.br.x / PROJECTMuL);
			maxY = (Int32)(route->bounds.br.y / PROJECTMuL);
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
			route = this->routeMap.GetItem(i);
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
	NotNullPtr<Data::ArrayList<Int32>> index;
	if (index.Set(this->indexMap.Get(key)))
	{
		routeList->AddAll(index);
	}
}

Map::HKSpeedLimit::HKSpeedLimit(Map::HKRoadNetwork2 *roadNetwork)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	RouteInfo *route;
	UOSInt i;
	this->dataCsys = roadNetwork->CreateCoordinateSystem();
	this->reqCsys = 0;
	DB::ReadingDB *fgdb = roadNetwork->GetDB();
	if (fgdb)
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
					route->vecOri = r->GetVector(shapeCol);
					route->bounds = route->vecOri->GetBounds();
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
								Text::StrToInt32(sbuff, route->speedLimit);
							}
						}
					}
				}
				fgdb->CloseReader(r);
			}

			Math::RectAreaDbl bounds;
			i = this->routeMap.GetCount();
			i--;
			route = this->routeMap.GetItem(i);
			bounds = route->bounds;
			while (i-- > 0)
			{
				route = this->routeMap.GetItem(i);
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
	DEL_CLASS(this->dataCsys);
	SDEL_CLASS(this->reqCsys);
	UOSInt i = this->routeMap.GetCount();
	while (i-- > 0) FreeRoute(this->routeMap.GetItem(i));
	this->FreeIndex();
}

const Map::HKSpeedLimit::RouteInfo *Map::HKSpeedLimit::GetNearestRoute(Math::Coord2DDbl pt)
{
	if (this->reqCsys && !this->reqCsys->Equals(this->dataCsys))
	{
		pt = Math::CoordinateSystem::Convert(this->reqCsys, this->dataCsys, pt);
	}
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
		xInd = (Int32)(pt.x / PROJECTMuL);
		yInd = (Int32)(pt.y / PROJECTMuL);
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
	ArtificialQuickSort_SortInt32(routeList.Ptr(), 0, (OSInt)routeList.GetCount() - 1);
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
				thisDist = route->vecOri->CalSqrDistance(pt, 0);
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

Int32 Map::HKSpeedLimit::GetSpeedLimit(Math::Coord2DDbl pt, Double maxDistM)
{
	Int32 speedLimit = 50;
	Double distComp = maxDistM * maxDistM;

	if (this->reqCsys && !this->reqCsys->Equals(this->dataCsys))
	{
		pt = Math::CoordinateSystem::Convert(this->reqCsys, this->dataCsys, pt);
	}
	Data::ArrayList<Int32> routeList;
	UOSInt i;
	RouteInfo *route;
	Double thisDist;
	Int32 lastRouteId = -1;
	Int32 routeId;
	Int32 xInd;
	Int32 yInd;
	if (this->bounds.br.x > 180 || this->bounds.br.y > 90 || this->bounds.tl.x < -180 || this->bounds.tl.y < -90)
	{
		xInd = (Int32)(pt.x / PROJECTMuL);
		yInd = (Int32)(pt.y / PROJECTMuL);
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
	ArtificialQuickSort_SortInt32(routeList.Ptr(), 0, (OSInt)routeList.GetCount() - 1);
	i = routeList.GetCount();
	while (i-- > 0)
	{
		routeId = routeList.GetItem(i);
		if (routeId != lastRouteId)
		{
			lastRouteId = routeId;
			route = this->routeMap.Get(routeId);
			if (route && route->speedLimit > speedLimit)
			{
				thisDist = route->vecOri->CalSqrDistance(pt, 0);
				if (thisDist <= distComp)
				{
					speedLimit = route->speedLimit;
				}
			}
		}
	}

	return speedLimit;
}

void Map::HKSpeedLimit::SetReqCoordinateSystem(Math::CoordinateSystem *csys)
{
	if (csys && (this->reqCsys == 0 || !this->reqCsys->Equals(csys)))
	{
		SDEL_CLASS(this->reqCsys);
		this->reqCsys = csys->Clone();
	}

}
