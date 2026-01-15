#include "Stdafx.h"
#include "Map/HKTDVehRestrict.h"
#include "Map/VectorLayer.h"
#include "Math/Math_C.h"
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
	UIntOS colCnt;
	UIntOS i;
	UIntOS j;
	IntOS idCol = -1;
	NN<RouteInfo> route;
	NN<Math::Geometry::Vector2D> vec;

	routeLyr->GetAllObjectIds(idArr, nameArr);
	colCnt = routeLyr->GetColumnCnt();
	i = 0;
	while (i < colCnt)
	{
		if (routeLyr->GetColumnName(sbuff, i).SetTo(sptr))
		{
			if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("ROUTE_ID")))
			{
				idCol = (IntOS)i;
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
			if (routeLyr->GetString(sb, nameArr, idArr.GetItem(i), (UIntOS)idCol))
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
	UIntOS i;
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
	UIntOS colSize[] = {11, 32, 255};
	UIntOS colDP[] = {0, 10, 0};
	NN<Map::VectorLayer> lyr;
	NEW_CLASSNN(lyr, Map::VectorLayer(layerType, CSTR("HKTDVehRestirct"), 3, colNames, this->csys->Clone(), colTypes, colSize, colDP, 0, CSTR("VehRestrict")));
	
	NN<DB::DBReader> r;
	if (this->db->QueryTableData(nullptr, CSTR("VEHICLE_RESTRICTION"), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		UTF8Char sbuff2[64];
		IntOS roadRouteIdCol = -1;
		IntOS locationCol = -1;
		IntOS vrIdCol = -1;
		IntOS maxWeightCol = -1;
		IntOS remarksCol = -1;
		UIntOS i;
		UIntOS j;
		UnsafeArray<UTF8Char> strs[3];
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if (r->GetName(i, sbuff).SetTo(sptr))
			{
				if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("ROAD_ROUTE_ID")))
				{
					roadRouteIdCol = (IntOS)i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("LOCATION")))
				{
					locationCol = (IntOS)i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("VR_ID")))
				{
					vrIdCol = (IntOS)i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("MAX_WEIGHT")))
				{
					maxWeightCol = (IntOS)i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("REMARKS")))
				{
					remarksCol = (IntOS)i;
				}
			}
			i++;
		}

		while (r->ReadNext())
		{
			if (r->IsNull((UIntOS)maxWeightCol))
			{
			}
			else
			{
				Int32 roadRouteId = r->GetInt32((UIntOS)roadRouteIdCol);
				Double location = r->GetDblOr((UIntOS)locationCol, 0);
				Int32 vrId = r->GetInt32((UIntOS)vrIdCol);
				Double maxWeight = r->GetDblOr((UIntOS)maxWeightCol, 0);
				NN<RouteInfo> route;
				NN<Math::Geometry::Point> pt;
				Math::Coord2DDbl coord;
				sbuff[0] = 0;
				r->GetStr((UIntOS)remarksCol, sbuff, sizeof(sbuff));

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
					lyr->AddVector2(pt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(UARR(strs)));
				}
			}
		}

		this->db->CloseReader(r);
	}
	return lyr;
}
