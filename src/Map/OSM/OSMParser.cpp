#include "Stdafx.h"
#include "Data/FastMap.h"
#include "Map/MapLayerCollection.h"
#include "Map/VectorLayer.h"
#include "Map/OSM/OSMParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"
#include "Text/MyStringFloat.h"

#define OSMTYPECNT 80

typedef struct
{
	Int64 id;
	Double lat;
	Double lon;
} OSMNodeInfo;

Map::MapDrawLayer *Map::OSM::OSMParser::ParseLayerNode(NN<Text::XMLReader> reader, Text::CStringNN fileName)
{
	Data::FastMap<Int64, OSMNodeInfo*> nodeMap;
	OSMNodeInfo *node;
	Data::ArrayList<Double> latList;
	Data::ArrayList<Double> lonList;
	Map::VectorLayer *layers[OSMTYPECNT];
	UnsafeArrayOpt<const UTF8Char> colName[2] = {(const UTF8Char*)"Name", (const UTF8Char*)"Elevation"};
	UnsafeArrayOpt<const UTF8Char> pgName[2] = {(const UTF8Char*)"Name", (const UTF8Char*)"Type"};
	static Text::CString layerNames[OSMTYPECNT] = {
		CSTR("Coastline"),
		CSTR("ResidentialArea"),
		CSTR("CommercialArea"),
		CSTR("IndustrialArea"),
		CSTR("Park"), //5
		CSTR("SchoolArea"),
		CSTR("ParkingArea"),
		CSTR("RiverBank"),
		CSTR_NULL,
		CSTR("Pool"), //10
		CSTR("Stream"),
		CSTR_NULL,
		CSTR_NULL,
		CSTR_NULL,
		CSTR("Railline"), //15
		CSTR("Centerline"),
		CSTR("Footpath"),
		CSTR("Building"),
		CSTR("Cycleway"),
		CSTR("Test"), //20
		CSTR("WireFence"),
		CSTR("GardenCentre"),
		CSTR_NULL,
		CSTR_NULL,
		CSTR_NULL, //25
		CSTR_NULL,
		CSTR_NULL,
		CSTR_NULL,
		CSTR_NULL,
		CSTR_NULL, //30
		CSTR("GuidePost"),
		CSTR("InfoMap"),
		CSTR("LiftGate"),
		CSTR("GraveYard"),
		CSTR("MotorwayJunction"), //35
		CSTR("Shelter"),
		CSTR("Toilet"),
		CSTR("Peak"),
		CSTR("Village"),
		CSTR("Telephone"), //40
		CSTR("BusStation"),
		CSTR("PlaceOfWorship"),
		CSTR("Place"),
		CSTR("Parking"),
		CSTR("Clock"), //45
		CSTR("Taxi"),
		CSTR("Gate"),
		CSTR("Crossing"),
		CSTR("TrafficSignal"),
		CSTR("TurningCircle"), //50
		CSTR("Viewpoint"),
		CSTR("Campsite"),
		CSTR("RailwayStation"),
		CSTR("PowerStation"),
		CSTR("Supermarket"), //55
		CSTR("UnusedMine"),
		CSTR("BBQArea"),
		CSTR("Rock"),
		CSTR("BoundaryStone"),
		CSTR("Pier"), //60
		CSTR("Marina"),
		CSTR("SwimmingPool"),
	};
	NN<Text::XMLAttrib> attr;
	UOSInt i;
	Int64 id;
	Double lat;
	Double lon;
	NN<Text::String> nodeText;
	NN<Text::String> aname;
	NN<Text::String> avalue;
	i = OSMTYPECNT;
	while (i-- > 0)
	{
		layers[i] = 0;
	}
	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("node")))
		{
			id = 0;
			lat = 0;
			lon = 0;
			i = reader->GetAttribCount();
			while (i-- > 0)
			{
				attr = reader->GetAttribNoCheck(i);
				aname = Text::String::OrEmpty(attr->name);
				if (aname->Equals(UTF8STRC("id")) && attr->value.SetTo(avalue))
				{
					id = avalue->ToInt64();
				}
				else if (aname->Equals(UTF8STRC("lat")) && attr->value.SetTo(avalue))
				{
					lat = avalue->ToDoubleOrNAN();
				}
				else if (aname->Equals(UTF8STRC("lon")) && attr->value.SetTo(avalue))
				{
					lon = avalue->ToDoubleOrNAN();
				}
			}
			if (id != 0 && lat != 0 && lon != 0)
			{
				node = MemAlloc(OSMNodeInfo, 1);
				node->id = id;
				node->lat = lat;
				node->lon = lon;
				node = nodeMap.Put(node->id, node);
				if (node)
				{
					MemFree(node);
				}

				if (!reader->IsElementEmpty())
				{
					Int32 elemType = 0;
					Text::String *names[2] = {0, 0};
					Optional<Text::String> kName;
					Optional<Text::String> vName;
					NN<Text::String> kNameStr;
					NN<Text::String> vNameStr;
					while (reader->NextElementName().SetTo(nodeText))
					{
						if (nodeText->Equals(UTF8STRC("tag")))
						{
							kName = 0;
							vName = 0;
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttribNoCheck(i);
								aname = Text::String::OrEmpty(attr->name);
								if (aname->Equals(UTF8STRC("k")))
								{
									kName = attr->value;
								}
								else if (aname->Equals(UTF8STRC("v")))
								{
									vName = attr->value;
								}
							}
							if (kName.SetTo(kNameStr) && vName.SetTo(vNameStr))
							{
								if (kNameStr->Equals(UTF8STRC("name")))
								{
									SDEL_STRING(names[0]);
									names[0] = vNameStr->Clone().Ptr();
								}
								else if (kNameStr->StartsWith(UTF8STRC("name:")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("ref")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("ele")))
								{
									SDEL_STRING(names[1]);
									names[1] = vNameStr->Clone().Ptr();
								}
								else if (kNameStr->Equals(UTF8STRC("display")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("foot")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("maxtents")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("wikipedia")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("mineral")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("region")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("type")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("platforms")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("wheelchair")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("direction")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("brand")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("operator")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("bicycle")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("crossing")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("cuisine")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("website")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("note")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("locked")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("motorcar")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("motorcycle")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("access")))
								{
								}
								else if (kNameStr->StartsWith(UTF8STRC("seamark:")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("source")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("opening_hours")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("phone")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("fee")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("atm")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("emergency")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("denomination")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("natual")))
								{
								}
								else if (kNameStr->StartsWith(UTF8STRC("addr:")))
								{
								}
								else if (kNameStr->Equals(UTF8STRC("amenity")))
								{
									if (vNameStr->Equals(UTF8STRC("grave_yard")))
									{
										if (elemType == 0)
										{
											elemType = 34;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("shelter")))
									{
										if (elemType == 0)
										{
											elemType = 36;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("toilets")))
									{
										if (elemType == 0)
										{
											elemType = 37;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("telephone")))
									{
										if (elemType == 0)
										{
											elemType = 40;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("bus_station")))
									{
										if (elemType == 0)
										{
											elemType = 41;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("place_of_worship")))
									{
										if (elemType == 0)
										{
											elemType = 42;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("parking")))
									{
										if (elemType == 0)
										{
											elemType = 44;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("clock")))
									{
										if (elemType == 0)
										{
											elemType = 45;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("taxi")))
									{
										if (elemType == 0)
										{
											elemType = 46;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("bbq")))
									{
										if (elemType == 0)
										{
											elemType = 57;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("fast_food")))
									{
									}
									else if (vNameStr->Equals(UTF8STRC("bench")))
									{
									}
									else if (vNameStr->Equals(UTF8STRC("waste_basket")))
									{
									}
								}
								else if (kNameStr->Equals(UTF8STRC("barrier")))
								{
									if (vNameStr->Equals(UTF8STRC("lift_gate")))
									{
										if (elemType == 0)
										{
											elemType = 33;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("gate")))
									{
										if (elemType == 0)
										{
											elemType = 47;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("bollard")))
									{
									}
								}
								else if (kNameStr->Equals(UTF8STRC("building")))
								{
									if (vNameStr->Equals(UTF8STRC("residential")))
									{
									}
								}
								else if (kNameStr->Equals(UTF8STRC("generator:source")))
								{
									if (vNameStr->Equals(UTF8STRC("wind")))
									{
										if (elemType == 0)
										{
											elemType = 54;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("highway")))
								{
									if (vNameStr->Equals(UTF8STRC("motorway_junction")))
									{
										if (elemType == 0)
										{
											elemType = 35;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("crossing")))
									{
										if (elemType == 0)
										{
											elemType = 48;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("traffic_signals")))
									{
										if (elemType == 0)
										{
											elemType = 49;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("turning_circle")))
									{
										if (elemType == 0)
										{
											elemType = 50;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("bus_stop")))
									{
										if (elemType == 0)
										{
											elemType = 41;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("mini_roundabout")))
									{
										if (elemType == 0)
										{
											elemType = 50;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("historic")))
								{
									if (vNameStr->Equals(UTF8STRC("mine")))
									{
										if (elemType == 0)
										{
											elemType = 56;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("boundary_stone")))
									{
										if (elemType == 0)
										{
											elemType = 59;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("information")))
								{
									if (vNameStr->Equals(UTF8STRC("guidepost")))
									{
										if (elemType == 0)
										{
											elemType = 31;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("map")))
									{
										if (elemType == 0)
										{
											elemType = 32;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("landuse")))
								{
									if (vNameStr->Equals(UTF8STRC("railway")))
									{
										if (elemType == 0)
										{
											elemType = 53;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("leisure")))
								{
									if (vNameStr->Equals(UTF8STRC("marina")))
									{
										if (elemType == 0)
										{
											elemType = 61;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("man_made")))
								{
									if (vNameStr->Equals(UTF8STRC("pier")))
									{
										if (elemType == 0)
										{
											elemType = 60;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("natural")))
								{
									if (vNameStr->Equals(UTF8STRC("peak")))
									{
										if (elemType == 0)
										{
											elemType = 38;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("rock")))
									{
										if (elemType == 0)
										{
											elemType = 58;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("parking")))
								{
									if (vNameStr->Equals(UTF8STRC("multi-storey")))
									{
										if (elemType == 0)
										{
											elemType = 44;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("place")))
								{
									if (vNameStr->Equals(UTF8STRC("village")))
									{
										if (elemType == 0)
										{
											elemType = 39;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("locality")))
									{
										if (elemType == 0)
										{
											elemType = 43;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("suburb")))
									{
										if (elemType == 0)
										{
											elemType = 43;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("power")))
								{
									if (vNameStr->Equals(UTF8STRC("generator")))
									{
										if (elemType == 0)
										{
											elemType = 54;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("railway")))
								{
									if (vNameStr->Equals(UTF8STRC("station")))
									{
										if (elemType == 0)
										{
											elemType = 53;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("subway_entrance")))
									{
									}
								}
								else if (kNameStr->Equals(UTF8STRC("religion")))
								{
									if (vNameStr->Equals(UTF8STRC("taoist")))
									{
										if (elemType == 0)
										{
											elemType = 42;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("shop")))
								{
									if (vNameStr->Equals(UTF8STRC("supermarket")))
									{
										if (elemType == 0)
										{
											elemType = 55;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("sport")))
								{
									if (vNameStr->Equals(UTF8STRC("swimming")))
									{
										if (elemType == 0)
										{
											elemType = 62;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("tourism")))
								{
									if (vNameStr->Equals(UTF8STRC("information")))
									{
									}
									else if (vNameStr->Equals(UTF8STRC("viewpoint")))
									{
										if (elemType == 0)
										{
											elemType = 51;
										}
									}
									else if (vNameStr->Equals(UTF8STRC("camp_site")))
									{
										if (elemType == 0)
										{
											elemType = 52;
										}
									}
								}
								else if (kNameStr->Equals(UTF8STRC("traffic_calming")))
								{
									if (vNameStr->Equals(UTF8STRC("rumble_strip")))
									{
									}
								}
								else
								{
									i = 0;
								}
							}
							reader->SkipElement();
						}
						else
						{
							reader->SkipElement();
						}
					}
					if (elemType > 0)
					{
						if (layers[elemType - 1] == 0)
						{
							UOSInt colCnt = 1;
							if (elemType == 38)
							{
								colCnt = 2;
							}
							NEW_CLASS(layers[elemType - 1], Map::VectorLayer(Map::DRAW_LAYER_POINT, fileName, colCnt, colName, Math::CoordinateSystemManager::CreateWGS84Csys(), 0, layerNames[elemType - 1]));
						}
						NN<Math::Geometry::Point> pt;
						NEW_CLASSNN(pt, Math::Geometry::Point(4326, lon, lat));
						layers[elemType - 1]->AddVector(pt, names);
					}
					SDEL_STRING(names[0]);
					SDEL_STRING(names[1]);
				}
			}
			else
			{
				reader->SkipElement();
			}
		}
		else if (nodeText->Equals(UTF8STRC("way")))
		{
			Int32 elemType = 0;
			Text::String *names[2] = {0, 0};
			Optional<Text::String> kName;
			Optional<Text::String> vName;
			NN<Text::String> kNameStr;
			NN<Text::String> vNameStr;
			Int64 nodeId;
			Bool isErr = false;
			Bool isPG = false;
			latList.Clear();
			lonList.Clear();
			while (reader->NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("nd")))
				{
					i = reader->GetAttribCount();
					while (i-- > 0)
					{
						attr = reader->GetAttribNoCheck(i);
						aname = Text::String::OrEmpty(attr->name);
						if (aname->Equals(UTF8STRC("ref")) && attr->value.SetTo(avalue))
						{
							nodeId = avalue->ToInt64();
							node = nodeMap.Get(nodeId);
							if (node == 0)
							{
								isErr = true;
							}
							else
							{
								latList.Add(node->lat);
								lonList.Add(node->lon);
							}
							break;
						}
					}
					reader->SkipElement();
				}
				else if (nodeText->Equals(UTF8STRC("tag")))
				{
					kName = 0;
					vName = 0;
					i = reader->GetAttribCount();
					while (i-- > 0)
					{
						attr = reader->GetAttribNoCheck(i);
						aname = Text::String::OrEmpty(attr->name);
						if (aname->Equals(UTF8STRC("k")))
						{
							kName = attr->value;
						}
						else if (aname->Equals(UTF8STRC("v")))
						{
							vName = attr->value;
						}
					}
					if (kName.SetTo(kNameStr) && vName.SetTo(vNameStr))
					{
						if (kNameStr->Equals(UTF8STRC("name")))
						{
							SDEL_STRING(names[0]);
							names[0] = vNameStr->Clone().Ptr();
						}
						else if (kNameStr->StartsWith(UTF8STRC("name:")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("oneway")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("bridge")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("addr:housenumber")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("building:levels")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("layer")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("psv")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("vehicle")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("bicycle")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("foot")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("surface")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("segregated")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("gauge")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("wikipedia")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("access")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("ref")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("source")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("parking")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("emergency")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("wheelchair")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("tunnel")))
						{
						}
						else if (kNameStr->StartsWith(UTF8STRC("alt_name")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("man_made")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("sport")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("motor_vehicle")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("electrified")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("voltage")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("width")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("covered")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("website")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("religion")))
						{
						}
						else if (kNameStr->Equals(UTF8STRC("amenity")))
						{
							if (vNameStr->Equals(UTF8STRC("school")))
							{
								if (elemType == 0)
								{
									elemType = 6;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("School")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("parking")))
							{
								if (elemType == 0)
								{
									elemType = 7;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Parking")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("hospital")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Hospital")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("college")))
							{
								if (elemType == 0)
								{
									elemType = 6;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("College")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("place_of_worship")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("PlaceOfWorship")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("nursing_home")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("NursingHome")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("community_centre")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("CommunityCentre")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("university")))
							{
								if (elemType == 0)
								{
									elemType = 6;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("University")).Ptr();
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("barrier")))
						{
							if (vNameStr->Equals(UTF8STRC("wire_fence")))
							{
								if (elemType == 0)
								{
									elemType = 21;
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("building")))
						{
							if (vNameStr->Equals(UTF8STRC("yes")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Building")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("house")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("House")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("school")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("School")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("apartments")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Apartments")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("residential")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Residential")).Ptr();
								}
							}
							else
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Unknown")).Ptr();
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("highway")))
						{
							if (vNameStr->Equals(UTF8STRC("service")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Service")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("secondary")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Secondary")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("footway")))
							{
								if (elemType == 0)
								{
									elemType = 17;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Footway")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("motorway")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Motorway")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("motorway_link")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("MotorwayLink")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("residential")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Residential")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("tertiary")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Tertiary")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("path")))
							{
								if (elemType == 0)
								{
									elemType = 17;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Path")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("primary")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Primary")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("steps")))
							{
								if (elemType == 0)
								{
									elemType = 17;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Steps")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("cycleway")))
							{
								if (elemType == 0)
								{
									elemType = 19;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Cycleway")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("pedestrian")))
							{
								if (elemType == 0)
								{
									elemType = 17;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Pedestrian")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("track")))
							{
								if (elemType == 0)
								{
									elemType = 16;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Pedestrian")).Ptr();
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("landuse")))
						{
							if (vNameStr->Equals(UTF8STRC("reservoir")))
							{
								if (elemType == 0)
								{
									elemType = 10;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Reservoir")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("residential")))
							{
								if (elemType == 0)
								{
									elemType = 2;
									isPG = true;
								}
							}
							else if (vNameStr->Equals(UTF8STRC("commercial")))
							{
								if (elemType == 0)
								{
									elemType = 3;
									isPG = true;
								}
							}
							else if (vNameStr->Equals(UTF8STRC("industrial")))
							{
								if (elemType == 0)
								{
									elemType = 4;
									isPG = true;
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("leisure")))
						{
							if (vNameStr->Equals(UTF8STRC("sports_centre")))
							{
								if (elemType == 0)
								{
									elemType = 5;
									isPG = true;
								}
							}
							else if (vNameStr->Equals(UTF8STRC("park")))
							{
								if (elemType == 0)
								{
									elemType = 5;
									isPG = true;
								}
							}
							else if (vNameStr->Equals(UTF8STRC("pitch")))
							{
								if (elemType == 0)
								{
									elemType = 5;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Pitch")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("swimming_pool")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("SwimmingPool")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("track")))
							{
								if (elemType == 0)
								{
									elemType = 5;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Track")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("stadium")))
							{
								if (elemType == 0)
								{
									elemType = 5;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Stadium")).Ptr();
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("natural")))
						{
							if (vNameStr->Equals(UTF8STRC("coastline")))
							{
								if (elemType == 0)
								{
									elemType = 1;
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("power")))
						{
							if (vNameStr->Equals(UTF8STRC("sub_station")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("PowerSubStation")).Ptr();
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("railway")))
						{
							if (vNameStr->Equals(UTF8STRC("rail")))
							{
								if (elemType == 0)
								{
									elemType = 15;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Rail")).Ptr();
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("shop")))
						{
							if (vNameStr->Equals(UTF8STRC("garden_centre")))
							{
								if (elemType == 0)
								{
									elemType = 22;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("GardenCentre")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("mall")))
							{
								if (elemType == 0)
								{
									elemType = 18;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Mall")).Ptr();
								}
							}
						}
						else if (kNameStr->Equals(UTF8STRC("waterway")))
						{
							if (vNameStr->Equals(UTF8STRC("stream")))
							{
								if (elemType == 0)
								{
									elemType = 11;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Stream")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("weir")))
							{
								if (elemType == 0)
								{
									elemType = 0;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Weir")).Ptr();
								}
							}
							else if (vNameStr->Equals(UTF8STRC("riverbank")))
							{
								if (elemType == 0)
								{
									elemType = 8;
									isPG = true;
									SDEL_STRING(names[1]);
									names[1] = Text::String::New(UTF8STRC("Riverbank")).Ptr();
								}
							}
						}
						else
						{
							i = 0;
						}
					}
					reader->SkipElement();
				}
				else
				{
					reader->SkipElement();
				}
			}
			if (elemType > 0 && !isErr)
			{
				if (isPG)
				{
					if (layers[elemType - 1] == 0)
					{
						UOSInt colCnt = 1;
						if (elemType == 6)
						{
							colCnt = 2;
						}
						NEW_CLASS(layers[elemType - 1], Map::VectorLayer(Map::DRAW_LAYER_POLYGON, fileName, colCnt, pgName, Math::CoordinateSystemManager::CreateWGS84Csys(), 0, layerNames[elemType - 1]));
					}
					NN<Math::Geometry::Polygon> pg;
					NN<Math::Geometry::LinearRing> lr;
					NEW_CLASSNN(pg, Math::Geometry::Polygon(4326));
					NEW_CLASSNN(lr, Math::Geometry::LinearRing(4326, latList.GetCount(), false, false));
					UnsafeArray<Math::Coord2DDbl> points = lr->GetPointList(i);
					while (i-- > 0)
					{
						points[i].x = lonList.GetItem(i);
						points[i].y = latList.GetItem(i);
					}
					pg->AddGeometry(lr);
					layers[elemType - 1]->AddVector(pg, names);
				}
				else
				{
					if (layers[elemType - 1] == 0)
					{
						UOSInt colCnt = 1;
						if (elemType == 6)
						{
							colCnt = 2;
						}
						NEW_CLASS(layers[elemType - 1], Map::VectorLayer(Map::DRAW_LAYER_POLYLINE, fileName, colCnt, pgName, Math::CoordinateSystemManager::CreateWGS84Csys(), 0, layerNames[elemType - 1]));
					}
					NN<Math::Geometry::LineString> pl;
					NEW_CLASSNN(pl, Math::Geometry::LineString(4326, latList.GetCount(), false, false));
					UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
					while (i-- > 0)
					{
						points[i].x = lonList.GetItem(i);
						points[i].y = latList.GetItem(i);
					}
					layers[elemType - 1]->AddVector(pl, names);
				}
			}
			SDEL_STRING(names[0]);
			SDEL_STRING(names[1]);
		}
		else if (nodeText->Equals(UTF8STRC("relation")))
		{
			reader->SkipElement();
		}
		else
		{
			reader->SkipElement();
		}
	}
	i = nodeMap.GetCount();
	while (i-- > 0)
	{
		MemFree(nodeMap.GetItem(i));
	}
	Map::MapLayerCollection *layerList;
	NN<Map::MapDrawLayer> layer;
	NEW_CLASS(layerList, Map::MapLayerCollection(fileName, CSTR("OSM")));
	i = 0;
	while (i < OSMTYPECNT)
	{
		if (layer.Set(layers[i]))
		{
			layerList->Add(layer);
		}
		i++;
	}
	return layerList;
}
