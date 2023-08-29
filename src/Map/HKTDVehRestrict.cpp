#include "Stdafx.h"
#include "Map/HKTDVehRestrict.h"
#include "Map/VectorLayer.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Text/MyStringFloat.h"

Map::HKTDVehRestrict::HKTDVehRestrict(Map::MapDrawLayer *routeLyr, DB::DBTool *db)
{
	this->db = db;
	this->csys = routeLyr->GetCoordinateSystem()->Clone();
	Map::GetObjectSess *sess = routeLyr->BeginGetObject();
	if (sess)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		Map::NameArray *nameArr;
		Data::ArrayListInt64 idArr;
		UOSInt colCnt;
		UOSInt i;
		UOSInt j;
		OSInt idCol = -1;
		RouteInfo *route;
		Math::Geometry::Vector2D *vec;

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
					vec = routeLyr->GetNewVectorById(sess, idArr.GetItem(i));
					if (vec)
					{
						if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline)
						{
							route = MemAlloc(RouteInfo, 1);
							route->routeId = Text::StrToInt32(sbuff);
							route->pl = (Math::Geometry::Polyline*)vec;
							route = this->routeMap.Put(route->routeId, route);
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

	i = this->routeMap.GetCount();
	while (i-- > 0)
	{
		route = this->routeMap.GetItem(i);
		DEL_CLASS(route->pl);
		MemFree(route);
	}
}

Map::MapDrawLayer *Map::HKTDVehRestrict::CreateTonnesSignLayer()
{
	Map::DrawLayerType layerType = Map::DRAW_LAYER_POINT;
	const UTF8Char *colNames[] = {(const UTF8Char*)"Id", (const UTF8Char*)"MaxWeight", (const UTF8Char*)"Remarks"};
	DB::DBUtil::ColType colTypes[] = {DB::DBUtil::CT_Int32, DB::DBUtil::CT_Double, DB::DBUtil::CT_VarUTF8Char};
	UOSInt colSize[] = {11, 32, 255};
	UOSInt colDP[] = {0, 10, 0};
	Map::VectorLayer *lyr = 0;
	NEW_CLASS(lyr, Map::VectorLayer(layerType, CSTR("HKTDVehRestirct"), 3, colNames, this->csys->Clone(), colTypes, colSize, colDP, 0, CSTR("VehRestrict")));
	
	DB::DBReader *r = this->db->QueryTableData(CSTR_NULL, CSTR("VEHICLE_RESTRICTION"), 0, 0, 0, CSTR_NULL, 0);
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
				Math::Geometry::Point *pt;
				Double ptX;
				Double ptY;
				Double dist;
				Double diffX;
				Double diffY;
				sbuff[0] = 0;
				r->GetStr((UOSInt)remarksCol, sbuff, sizeof(sbuff));

				route = this->routeMap.Get(roadRouteId);
				if (route)
				{
					Math::Coord2DDbl *points = route->pl->GetPointList(j);
					if (location <= 0)
					{
						ptX = points[0].x;
						ptY = points[0].y;
					}
					else
					{
						ptX = points[j - 1].x;
						ptY = points[j - 1].y;

						i = 1;
						while (i < j)
						{
							diffX = points[i - 1].x - points[i].x;
							diffY = points[i - 1].y - points[i].y;
							dist = Math_Sqrt(diffX * diffX + diffY * diffY);
							if (dist > location)
							{
								ptX = points[i - 1].x + (points[i].x - points[i - 1].x) * location / dist;
								ptY = points[i - 1].y + (points[i].y - points[i - 1].y) * location / dist;
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
					UInt32 iMaxWeight = (UInt32)maxWeight;
					if (maxWeight - iMaxWeight < 0.1)
					{
						Text::StrUInt32(strs[1], iMaxWeight);
					}
					else
					{
						Text::StrDouble(strs[1], maxWeight);
					}
					strs[2] = sbuff;
					NEW_CLASS(pt, Math::Geometry::Point(this->csys->GetSRID(), ptX, ptY));
					lyr->AddVector(pt, (const UTF8Char**)strs);
				}
			}
		}

		this->db->CloseReader(r);
	}
	return lyr;
}
