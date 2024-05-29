#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "DB/DBReader.h"
#include "Map/HKSpeedLimit.h"

#define PROJECTMuL 1000.0
#define LATLONMUL 40
#include <stdio.h>

void Map::HKSpeedLimit::FreeRoute(NN<RouteInfo> route)
{
	route->vecOri.Delete();
	route.Delete();
}

void Map::HKSpeedLimit::FreeIndex()
{
	NN<Data::ArrayList<Int32>> index;
	UOSInt i = this->indexMap.GetCount();
	while (i-- > 0)
	{
		index = this->indexMap.GetItemNoCheck(i);
		index.Delete();
	}
	this->indexMap.Clear();
}

void Map::HKSpeedLimit::BuildIndex()
{
	NN<RouteInfo> route;
	UOSInt i;
	NN<Data::ArrayList<Int32>> index;
	Int32 x;
	Int32 y;
	Int32 minX;
	Int32 maxX;
	Int32 minY;
	Int32 maxY;
	Int64 key;
	this->FreeIndex();
	i = this->routeMap.GetCount();
	if (this->bounds.max.x > 180 || this->bounds.max.y > 90 || this->bounds.min.x < -180 || this->bounds.min.y < -90)
	{
		while (i-- > 0)
		{
			route = this->routeMap.GetItemNoCheck(i);
			minX = (Int32)(route->bounds.min.x / PROJECTMuL);
			minY = (Int32)(route->bounds.min.y / PROJECTMuL);
			maxX = (Int32)(route->bounds.max.x / PROJECTMuL);
			maxY = (Int32)(route->bounds.max.y / PROJECTMuL);
			x = minX;
			while (x <= maxX)
			{
				y = minY;
				while (y <= maxY)
				{
					key = (((Int64)x) << 32) | (UInt32)y;
					if (!this->indexMap.Get(key).SetTo(index))
					{
						NEW_CLASSNN(index, Data::ArrayList<Int32>());
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
			route = this->routeMap.GetItemNoCheck(i);
			minX = (Int32)(route->bounds.min.x * LATLONMUL);
			minY = (Int32)(route->bounds.min.y * LATLONMUL);
			maxX = (Int32)(route->bounds.max.x * LATLONMUL);
			maxY = (Int32)(route->bounds.max.y * LATLONMUL);
			x = minX;
			while (x <= maxX)
			{
				y = minY;
				while (y <= maxY)
				{
					key = (((Int64)x) << 32) | (UInt32)y;
					if (!this->indexMap.Get(key).SetTo(index))
					{
						NEW_CLASSNN(index, Data::ArrayList<Int32>());
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

void Map::HKSpeedLimit::AppendRouteIds(NN<Data::ArrayList<Int32>> routeList, Int32 x, Int32 y)
{
	Int64 key = (((Int64)x) << 32) | (UInt32)y;
	NN<Data::ArrayList<Int32>> index;
	if (this->indexMap.Get(key).SetTo(index))
	{
		routeList->AddAll(index);
	}
}

Map::HKSpeedLimit::HKSpeedLimit(NN<Map::HKRoadNetwork2> roadNetwork)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NN<RouteInfo> route;
	UOSInt i;
	this->dataCsys = roadNetwork->CreateCoordinateSystem();
	this->reqCsys = 0;
	NN<DB::ReadingDB> fgdb;
	if (roadNetwork->GetDB().SetTo(fgdb))
	{
		NN<DB::DBReader> r;
		if (fgdb->QueryTableData(CSTR_NULL, CSTR("CENTERLINE"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
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
					NN<Math::Geometry::Vector2D> vec;
					NEW_CLASSNN(route, RouteInfo());
					route->objectId = r->GetInt32(objIdCol);
					route->routeId = r->GetInt32(routeIdCol);
					route->speedLimit = 50;
					route->vecOri = r->GetVector(shapeCol);
					if (route->vecOri.SetTo(vec))
						route->bounds = vec->GetBounds();
					else
						route->bounds = Math::RectAreaDbl(0, 0, 0, 0);
					if (this->routeMap.Put(route->routeId, route).SetTo(route))
					{
						FreeRoute(route);
					}
				}
			}
			fgdb->CloseReader(r);
		}
		if (this->routeMap.GetCount() > 0)
		{
			if (fgdb->QueryTableData(CSTR_NULL, CSTR("SPEED_LIMIT"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
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
						if (this->routeMap.Get(r->GetInt32(routeIdCol)).SetTo(route))
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
			route = this->routeMap.GetItemNoCheck(i);
			bounds = route->bounds;
			while (i-- > 0)
			{
				route = this->routeMap.GetItemNoCheck(i);
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
	this->dataCsys.Delete();
	this->reqCsys.Delete();
	this->routeMap.FreeAll(FreeRoute);
	this->FreeIndex();
}

Optional<const Map::HKSpeedLimit::RouteInfo> Map::HKSpeedLimit::GetNearestRoute(Math::Coord2DDbl pt)
{
	NN<Math::CoordinateSystem> csys;
	if (this->reqCsys.SetTo(csys) && !csys->Equals(this->dataCsys))
	{
		pt = Math::CoordinateSystem::Convert(csys, this->dataCsys, pt);
	}
	Data::ArrayList<Int32> routeList;
	UOSInt i;
	NN<RouteInfo> route;
	Int32 minRouteId = -1;
	Double minDist = 1000000000;
	Double thisDist;
	Int32 lastRouteId = -1;
	Int32 routeId;
	Int32 xInd;
	Int32 yInd;
	if (this->bounds.max.x > 180 || this->bounds.max.y > 90 || this->bounds.min.x < -180 || this->bounds.min.y < -90)
	{
		xInd = (Int32)(pt.x / PROJECTMuL);
		yInd = (Int32)(pt.y / PROJECTMuL);
	}
	else
	{
		xInd = (Int32)(pt.x * LATLONMUL);
		yInd = (Int32)(pt.y * LATLONMUL);
	}
	AppendRouteIds(routeList, xInd - 1, yInd - 1);
	AppendRouteIds(routeList, xInd + 0, yInd - 1);
	AppendRouteIds(routeList, xInd + 1, yInd - 1);
	AppendRouteIds(routeList, xInd - 1, yInd + 0);
	AppendRouteIds(routeList, xInd + 0, yInd + 0);
	AppendRouteIds(routeList, xInd + 1, yInd + 0);
	AppendRouteIds(routeList, xInd - 1, yInd + 1);
	AppendRouteIds(routeList, xInd + 0, yInd + 1);
	AppendRouteIds(routeList, xInd + 1, yInd + 1);
	ArtificialQuickSort_SortInt32(routeList.Arr().Ptr(), 0, (OSInt)routeList.GetCount() - 1);
	i = routeList.GetCount();
	while (i-- > 0)
	{
		routeId = routeList.GetItem(i);
		if (routeId != lastRouteId)
		{
			NN<Math::Geometry::Vector2D> vec;
			lastRouteId = routeId;
			if (this->routeMap.Get(routeId).SetTo(route) && route->vecOri.SetTo(vec))
			{
				Math::Coord2DDbl nearPt;
				thisDist = vec->CalSqrDistance(pt, nearPt);
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
	NN<Math::CoordinateSystem> csys;
	if (this->reqCsys.SetTo(csys) && !csys->Equals(this->dataCsys))
	{
		pt = Math::CoordinateSystem::Convert(csys, this->dataCsys, pt);
	}
	Data::ArrayList<Int32> routeList;
	UOSInt i;
	NN<RouteInfo> route;
	Double thisDist;
	Int32 lastRouteId = -1;
	Int32 routeId;
	Int32 xInd;
	Int32 yInd;
	if (this->bounds.max.x > 180 || this->bounds.max.y > 90 || this->bounds.min.x < -180 || this->bounds.min.y < -90)
	{
		xInd = (Int32)(pt.x / PROJECTMuL);
		yInd = (Int32)(pt.y / PROJECTMuL);
	}
	else
	{
		xInd = (Int32)(pt.x * LATLONMUL);
		yInd = (Int32)(pt.y * LATLONMUL);
	}
	AppendRouteIds(routeList, xInd - 1, yInd - 1);
	AppendRouteIds(routeList, xInd + 0, yInd - 1);
	AppendRouteIds(routeList, xInd + 1, yInd - 1);
	AppendRouteIds(routeList, xInd - 1, yInd + 0);
	AppendRouteIds(routeList, xInd + 0, yInd + 0);
	AppendRouteIds(routeList, xInd + 1, yInd + 0);
	AppendRouteIds(routeList, xInd - 1, yInd + 1);
	AppendRouteIds(routeList, xInd + 0, yInd + 1);
	AppendRouteIds(routeList, xInd + 1, yInd + 1);
	ArtificialQuickSort_SortInt32(routeList.Arr().Ptr(), 0, (OSInt)routeList.GetCount() - 1);
	i = routeList.GetCount();
	while (i-- > 0)
	{
		routeId = routeList.GetItem(i);
		if (routeId != lastRouteId)
		{
			NN<Math::Geometry::Vector2D> vec;
			lastRouteId = routeId;
			if (this->routeMap.Get(routeId).SetTo(route) && route->speedLimit > speedLimit && route->vecOri.SetTo(vec))
			{
				Math::Coord2DDbl nearPt;
				thisDist = vec->CalSqrDistance(pt, nearPt);
				if (thisDist <= distComp)
				{
					speedLimit = route->speedLimit;
				}
			}
		}
	}

	return speedLimit;
}

void Map::HKSpeedLimit::SetReqCoordinateSystem(NN<Math::CoordinateSystem> csys)
{
	NN<Math::CoordinateSystem> srcCsys = csys;
	NN<Math::CoordinateSystem> reqCsys;
	if (!this->reqCsys.SetTo(reqCsys) || !reqCsys->Equals(srcCsys))
	{
		this->reqCsys.Delete();
		this->reqCsys = csys->Clone();
	}
}
