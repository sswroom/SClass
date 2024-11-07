#include "Stdafx.h"
#include "Map/HKTDVehRestrict.h"
#include "Map/VectorLayer.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyStringFloat.h"

Map::HKTDVehRestrict::HKTDVehRestrict(NN<Map::MapDrawLayer> routeLyr, NN<DB::DBTool> db)
{
	this->db = db;
	this->csys = routeLyr->GetCoordinateSystem()->Clone();
	NN<Map::GetObjectSess> sess = routeLyr->BeginGetObject();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Map::NameArray> nameArr;
	Data::ArrayListInt64 idArr;
	UOSInt colCnt;
	UOSInt i;
	UOSInt j;
	OSInt idCol = -1;
	NN<RouteInfo> route;
	NN<Math::Geometry::Vector2D> vec;

	routeLyr->GetAllObjectIds(idArr, nameArr);
	colCnt = routeLyr->GetColumnCnt();
	i = 0;
	while (i < colCnt)
	{
		if (routeLyr->GetColumnName(sbuff, i).SetTo(sptr))
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
		Text::StringBuilderUTF8 sb;
		i = 0;
		j = idArr.GetCount();
		while (i < j)
		{
			sb.ClearStr();
			if (routeLyr->GetString(sb, nameArr, idArr.GetItem(i), (UOSInt)idCol))
			{
				if (routeLyr->GetNewVectorById(sess, idArr.GetItem(i)).SetTo(vec))
				{
					if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline)
					{
						route = MemAllocNN(RouteInfo);
						route->routeId = sb.ToInt32();
						route->pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
						if (this->routeMap.Put(route->routeId, route).SetTo(route))
						{
							route->pl.Delete();
							MemFreeNN(route);
						}
					}
					else
					{
						vec.Delete();
					}
				}
			}
			i++;
		}
	}

	routeLyr->ReleaseNameArr(nameArr);
	routeLyr->EndGetObject(sess);
	routeLyr.Delete();
}

Map::HKTDVehRestrict::~HKTDVehRestrict()
{
	UOSInt i;
	NN<RouteInfo> route;
	this->csys.Delete();
	this->db.Delete();

	i = this->routeMap.GetCount();
	while (i-- > 0)
	{
		route = this->routeMap.GetItemNoCheck(i);
		route->pl.Delete();
		MemFreeNN(route);
	}
}

NN<Map::MapDrawLayer> Map::HKTDVehRestrict::CreateTonnesSignLayer()
{
	Map::DrawLayerType layerType = Map::DRAW_LAYER_POINT;
	UnsafeArrayOpt<const UTF8Char> colNames[] = {(const UTF8Char*)"Id", (const UTF8Char*)"MaxWeight", (const UTF8Char*)"Remarks"};
	DB::DBUtil::ColType colTypes[] = {DB::DBUtil::CT_Int32, DB::DBUtil::CT_Double, DB::DBUtil::CT_VarUTF8Char};
	UOSInt colSize[] = {11, 32, 255};
	UOSInt colDP[] = {0, 10, 0};
	NN<Map::VectorLayer> lyr;
	NEW_CLASSNN(lyr, Map::VectorLayer(layerType, CSTR("HKTDVehRestirct"), 3, colNames, this->csys->Clone(), colTypes, colSize, colDP, 0, CSTR("VehRestrict")));
	
	NN<DB::DBReader> r;
	if (this->db->QueryTableData(CSTR_NULL, CSTR("VEHICLE_RESTRICTION"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		UTF8Char sbuff2[64];
		OSInt roadRouteIdCol = -1;
		OSInt locationCol = -1;
		OSInt vrIdCol = -1;
		OSInt maxWeightCol = -1;
		OSInt remarksCol = -1;
		UOSInt i;
		UOSInt j;
		UnsafeArray<UTF8Char> strs[3];
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if (r->GetName(i, sbuff).SetTo(sptr))
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
				Double location = r->GetDblOr((UOSInt)locationCol, 0);
				Int32 vrId = r->GetInt32((UOSInt)vrIdCol);
				Double maxWeight = r->GetDblOr((UOSInt)maxWeightCol, 0);
				NN<RouteInfo> route;
				NN<Math::Geometry::Point> pt;
				Math::Coord2DDbl coord;
				sbuff[0] = 0;
				r->GetStr((UOSInt)remarksCol, sbuff, sizeof(sbuff));

				if (this->routeMap.Get(roadRouteId).SetTo(route))
				{
					coord = route->pl->CalcPosAtDistance(location);
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
					NEW_CLASSNN(pt, Math::Geometry::Point(this->csys->GetSRID(), coord));
					lyr->AddVector(pt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(UARR(strs)));
				}
			}
		}

		this->db->CloseReader(r);
	}
	return lyr;
}
