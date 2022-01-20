#include "Stdafx.h"
#include "Map/HKTDVehRestrict.h"
#include "Map/VectorLayer.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Text/MyStringFloat.h"

Map::HKTDVehRestrict::HKTDVehRestrict(Map::IMapDrawLayer *routeLyr, DB::DBTool *db)
{
	this->db = db;
	this->csys = routeLyr->GetCoordinateSystem()->Clone();
	NEW_CLASS(this->routeMap, Data::Int32Map<RouteInfo*>());
	void *sess = routeLyr->BeginGetObject();
	if (sess)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		void *nameArr;
		Data::ArrayListInt64 idArr;
		UOSInt colCnt;
		UOSInt i;
		UOSInt j;
		OSInt idCol = -1;
		RouteInfo *route;
		Math::Vector2D *vec;

		routeLyr->GetAllObjectIds(&idArr, &nameArr);
		colCnt = routeLyr->GetColumnCnt();
		i = 0;
		while (i < colCnt)
		{
			if ((sptr = routeLyr->GetColumnName(sbuff, i)) != 0)
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ROUTE_ID")))
				{
					idCol = (OSInt)i;
					break;
				}
			}
			i++;
		}

		if (idCol != -1)
		{
			i = 0;
			j = idArr.GetCount();
			while (i < j)
			{
				if (routeLyr->GetString(sbuff, sizeof(sbuff), nameArr, idArr.GetItem(i), (UOSInt)idCol))
				{
					vec = routeLyr->GetVectorById(sess, idArr.GetItem(i));
					if (vec)
					{
						if (vec->GetVectorType() == Math::Vector2D::VectorType::Polyline)
						{
							route = MemAlloc(RouteInfo, 1);
							route->routeId = Text::StrToInt32(sbuff);
							route->pl = (Math::Polyline*)vec;
							route = this->routeMap->Put(route->routeId, route);
							if (route)
							{
								DEL_CLASS(route->pl);
								MemFree(route);
							}
						}
						else
						{
							DEL_CLASS(vec);
						}
					}
				}
				i++;
			}
		}

		routeLyr->ReleaseNameArr(nameArr);
		routeLyr->EndGetObject(sess);
	}
	DEL_CLASS(routeLyr);
}

Map::HKTDVehRestrict::~HKTDVehRestrict()
{
	UOSInt i;
	RouteInfo *route;
	DEL_CLASS(this->csys);
	DEL_CLASS(this->db);

	Data::ArrayList<RouteInfo*> *routeList = this->routeMap->GetValues();
	i = routeList->GetCount();
	while (i-- > 0)
	{
		route = routeList->GetItem(i);
		DEL_CLASS(route->pl);
		MemFree(route);
	}
	DEL_CLASS(this->routeMap);
}

Map::IMapDrawLayer *Map::HKTDVehRestrict::CreateTonnesSignLayer()
{
	Map::DrawLayerType layerType = Map::DRAW_LAYER_POINT;
	const UTF8Char *colNames[] = {(const UTF8Char*)"Id", (const UTF8Char*)"MaxWeight", (const UTF8Char*)"Remarks"};
	DB::DBUtil::ColType colTypes[] = {DB::DBUtil::CT_Int32, DB::DBUtil::CT_Double, DB::DBUtil::CT_VarChar};
	UOSInt colSize[] = {11, 32, 255};
	UOSInt colDP[] = {0, 10, 0};
	Map::VectorLayer *lyr = 0;
	NEW_CLASS(lyr, Map::VectorLayer(layerType, (const UTF8Char*)"HKTDVehRestirct", 3, colNames, this->csys->Clone(), colTypes, colSize, colDP, 0, (const UTF8Char*)"VehRestrict"));
	
	DB::DBReader *r = this->db->GetTableData((const UTF8Char*)"VEHICLE_RESTRICTION", 0, 0, 0, 0, 0);
	if (r)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UTF8Char sbuff2[64];
		OSInt roadRouteIdCol = -1;
		OSInt locationCol = -1;
		OSInt vrIdCol = -1;
		OSInt maxWeightCol = -1;
		OSInt remarksCol = -1;
		UOSInt i;
		UOSInt j;
		UTF8Char *strs[3];
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if ((sptr = r->GetName(i, sbuff)) != 0)
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ROAD_ROUTE_ID")))
				{
					roadRouteIdCol = (OSInt)i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LOCATION")))
				{
					locationCol = (OSInt)i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("VR_ID")))
				{
					vrIdCol = (OSInt)i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("MAX_WEIGHT")))
				{
					maxWeightCol = (OSInt)i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("REMARKS")))
				{
					remarksCol = (OSInt)i;
				}
			}
			i++;
		}

		while (r->ReadNext())
		{
			if (r->IsNull((UOSInt)maxWeightCol))
			{
			}
			else
			{
				Int32 roadRouteId = r->GetInt32((UOSInt)roadRouteIdCol);
				Double location = r->GetDbl((UOSInt)locationCol);
				Int32 vrId = r->GetInt32((UOSInt)vrIdCol);
				Double maxWeight = r->GetDbl((UOSInt)maxWeightCol);
				RouteInfo *route;
				Math::Point *pt;
				Double ptX;
				Double ptY;
				Double dist;
				Double diffX;
				Double diffY;
				sbuff[0] = 0;
				r->GetStr((UOSInt)remarksCol, sbuff, sizeof(sbuff));

				route = this->routeMap->Get(roadRouteId);
				if (route)
				{
					Double *points = route->pl->GetPointList(&j);
					if (location <= 0)
					{
						ptX = points[0];
						ptY = points[1];
					}
					else
					{
						ptX = points[j * 2 - 2];
						ptY = points[j * 2 - 1];

						i = 1;
						while (i < j)
						{
							diffX = points[i * 2 - 2] - points[i * 2];
							diffY = points[i * 2 - 1] - points[i * 2 + 1];
							dist = Math_Sqrt(diffX * diffX + diffY * diffY);
							if (dist > location)
							{
								ptX = points[i * 2 - 2] + (points[i * 2] - points[i * 2 - 2]) * location / dist;
								ptY = points[i * 2 - 1] + (points[i * 2 + 1] - points[i * 2 - 1]) * location / dist;
								break;
							}
							else
							{
								location -= dist;
							}
							i++;
						}
					}
					strs[0] = sbuff2;
					strs[1] = Text::StrInt32(sbuff2, vrId) + 1;
					Text::StrDouble(strs[1], maxWeight);
					strs[2] = sbuff;
					NEW_CLASS(pt, Math::Point(this->csys->GetSRID(), ptX, ptY));
					lyr->AddVector(pt, (const UTF8Char**)strs);
				}
			}
		}

		this->db->CloseReader(r);
	}
	return lyr;
}
