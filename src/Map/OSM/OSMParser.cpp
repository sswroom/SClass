#include "Stdafx.h"
#include "Data/Int64Map.h"
#include "Map/MapLayerCollection.h"
#include "Map/VectorLayer.h"
#include "Map/OSM/OSMParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Point.h"
#include "Text/MyStringFloat.h"

#define OSMTYPECNT 80

typedef struct
{
	Int64 id;
	Double lat;
	Double lon;
} OSMNodeInfo;

Map::IMapDrawLayer *Map::OSM::OSMParser::ParseLayerNode(Text::XMLReader *reader, const UTF8Char *fileName)
{
	Data::Int64Map<OSMNodeInfo*> nodeMap;
	OSMNodeInfo *node;
	Data::ArrayList<Double> latList;
	Data::ArrayList<Double> lonList;
	Map::VectorLayer *layers[OSMTYPECNT];
	const UTF8Char *colName[2] = {(const UTF8Char*)"Name", (const UTF8Char*)"Elevation"};
	const UTF8Char *pgName[2] = {(const UTF8Char*)"Name", (const UTF8Char*)"Type"};
	static const UTF8Char *layerNames[OSMTYPECNT] = {
		(const UTF8Char*)"Coastline",
		(const UTF8Char*)"ResidentialArea",
		(const UTF8Char*)"CommercialArea",
		(const UTF8Char*)"IndustrialArea",
		(const UTF8Char*)"Park", //5
		(const UTF8Char*)"SchoolArea",
		(const UTF8Char*)"ParkingArea",
		(const UTF8Char*)"RiverBank",
		0,
		(const UTF8Char*)"Pool", //10
		(const UTF8Char*)"Stream",
		0,
		0,
		0,
		(const UTF8Char*)"Railline", //15
		(const UTF8Char*)"Centerline",
		(const UTF8Char*)"Footpath",
		(const UTF8Char*)"Building",
		(const UTF8Char*)"Cycleway",
		(const UTF8Char*)"Test", //20
		(const UTF8Char*)"WireFence",
		(const UTF8Char*)"GardenCentre",
		0,
		0,
		0, //25
		0,
		0,
		0,
		0,
		0, //30
		(const UTF8Char*)"GuidePost",
		(const UTF8Char*)"InfoMap",
		(const UTF8Char*)"LiftGate",
		(const UTF8Char*)"GraveYard",
		(const UTF8Char*)"MotorwayJunction", //35
		(const UTF8Char*)"Shelter",
		(const UTF8Char*)"Toilet",
		(const UTF8Char*)"Peak",
		(const UTF8Char*)"Village",
		(const UTF8Char*)"Telephone", //40
		(const UTF8Char*)"BusStation",
		(const UTF8Char*)"PlaceOfWorship",
		(const UTF8Char*)"Place",
		(const UTF8Char*)"Parking",
		(const UTF8Char*)"Clock", //45
		(const UTF8Char*)"Taxi",
		(const UTF8Char*)"Gate",
		(const UTF8Char*)"Crossing",
		(const UTF8Char*)"TrafficSignal",
		(const UTF8Char*)"TurningCircle", //50
		(const UTF8Char*)"Viewpoint",
		(const UTF8Char*)"Campsite",
		(const UTF8Char*)"RailwayStation",
		(const UTF8Char*)"PowerStation",
		(const UTF8Char*)"Supermarket", //55
		(const UTF8Char*)"UnusedMine",
		(const UTF8Char*)"BBQArea",
		(const UTF8Char*)"Rock",
		(const UTF8Char*)"BoundaryStone",
		(const UTF8Char*)"Pier", //60
		(const UTF8Char*)"Marina",
		(const UTF8Char*)"SwimmingPool",
	};
	Text::XMLAttrib *attr;
	UOSInt i;
	Int64 id;
	Double lat;
	Double lon;
	i = OSMTYPECNT;
	while (i-- > 0)
	{
		layers[i] = 0;
	}
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (reader->GetNodeText()->Equals((const UTF8Char*)"node"))
			{
				id = 0;
				lat = 0;
				lon = 0;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals((const UTF8Char*)"id") && attr->value)
					{
						id = attr->value->ToInt64();
					}
					else if (attr->name->Equals((const UTF8Char*)"lat") && attr->value)
					{
						lat = attr->value->ToDouble();
					}
					else if (attr->name->Equals((const UTF8Char*)"lon") && attr->value)
					{
						lon = attr->value->ToDouble();
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
						Text::String *kName;
						Text::String *vName;
						while (reader->ReadNext())
						{
							if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
							{
								break;
							}
							else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals((const UTF8Char*)"tag"))
							{
								kName = 0;
								vName = 0;
								i = reader->GetAttribCount();
								while (i-- > 0)
								{
									attr = reader->GetAttrib(i);
									if (attr->name->Equals((const UTF8Char*)"k"))
									{
										kName = attr->value;
									}
									else if (attr->name->Equals((const UTF8Char*)"v"))
									{
										vName = attr->value;
									}
								}
								if (kName && vName)
								{
									if (kName->Equals((const UTF8Char*)"name"))
									{
										SDEL_STRING(names[0]);
										names[0] = vName->Clone();
									}
									else if (kName->StartsWith((const UTF8Char*)"name:"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"ref"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"ele"))
									{
										SDEL_STRING(names[1]);
										names[1] = vName->Clone();
									}
									else if (kName->Equals((const UTF8Char*)"display"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"foot"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"maxtents"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"wikipedia"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"mineral"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"region"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"type"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"platforms"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"wheelchair"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"direction"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"brand"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"operator"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"bicycle"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"crossing"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"cuisine"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"website"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"note"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"locked"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"motorcar"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"motorcycle"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"access"))
									{
									}
									else if (kName->StartsWith((const UTF8Char*)"seamark:"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"source"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"opening_hours"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"phone"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"fee"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"atm"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"emergency"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"denomination"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"natual"))
									{
									}
									else if (kName->StartsWith((const UTF8Char*)"addr:"))
									{
									}
									else if (kName->Equals((const UTF8Char*)"amenity"))
									{
										if (vName->Equals((const UTF8Char*)"grave_yard"))
										{
											if (elemType == 0)
											{
												elemType = 34;
											}
										}
										else if (vName->Equals((const UTF8Char*)"shelter"))
										{
											if (elemType == 0)
											{
												elemType = 36;
											}
										}
										else if (vName->Equals((const UTF8Char*)"toilets"))
										{
											if (elemType == 0)
											{
												elemType = 37;
											}
										}
										else if (vName->Equals((const UTF8Char*)"telephone"))
										{
											if (elemType == 0)
											{
												elemType = 40;
											}
										}
										else if (vName->Equals((const UTF8Char*)"bus_station"))
										{
											if (elemType == 0)
											{
												elemType = 41;
											}
										}
										else if (vName->Equals((const UTF8Char*)"place_of_worship"))
										{
											if (elemType == 0)
											{
												elemType = 42;
											}
										}
										else if (vName->Equals((const UTF8Char*)"parking"))
										{
											if (elemType == 0)
											{
												elemType = 44;
											}
										}
										else if (vName->Equals((const UTF8Char*)"clock"))
										{
											if (elemType == 0)
											{
												elemType = 45;
											}
										}
										else if (vName->Equals((const UTF8Char*)"taxi"))
										{
											if (elemType == 0)
											{
												elemType = 46;
											}
										}
										else if (vName->Equals((const UTF8Char*)"bbq"))
										{
											if (elemType == 0)
											{
												elemType = 57;
											}
										}
										else if (vName->Equals((const UTF8Char*)"fast_food"))
										{
										}
										else if (vName->Equals((const UTF8Char*)"bench"))
										{
										}
										else if (vName->Equals((const UTF8Char*)"waste_basket"))
										{
										}
									}
									else if (kName->Equals((const UTF8Char*)"barrier"))
									{
										if (vName->Equals((const UTF8Char*)"lift_gate"))
										{
											if (elemType == 0)
											{
												elemType = 33;
											}
										}
										else if (vName->Equals((const UTF8Char*)"gate"))
										{
											if (elemType == 0)
											{
												elemType = 47;
											}
										}
										else if (vName->Equals((const UTF8Char*)"bollard"))
										{
										}
									}
									else if (kName->Equals((const UTF8Char*)"building"))
									{
										if (vName->Equals((const UTF8Char*)"residential"))
										{
										}
									}
									else if (kName->Equals((const UTF8Char*)"generator:source"))
									{
										if (vName->Equals((const UTF8Char*)"wind"))
										{
											if (elemType == 0)
											{
												elemType = 54;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"highway"))
									{
										if (vName->Equals((const UTF8Char*)"motorway_junction"))
										{
											if (elemType == 0)
											{
												elemType = 35;
											}
										}
										else if (vName->Equals((const UTF8Char*)"crossing"))
										{
											if (elemType == 0)
											{
												elemType = 48;
											}
										}
										else if (vName->Equals((const UTF8Char*)"traffic_signals"))
										{
											if (elemType == 0)
											{
												elemType = 49;
											}
										}
										else if (vName->Equals((const UTF8Char*)"turning_circle"))
										{
											if (elemType == 0)
											{
												elemType = 50;
											}
										}
										else if (vName->Equals((const UTF8Char*)"bus_stop"))
										{
											if (elemType == 0)
											{
												elemType = 41;
											}
										}
										else if (vName->Equals((const UTF8Char*)"mini_roundabout"))
										{
											if (elemType == 0)
											{
												elemType = 50;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"historic"))
									{
										if (vName->Equals((const UTF8Char*)"mine"))
										{
											if (elemType == 0)
											{
												elemType = 56;
											}
										}
										else if (vName->Equals((const UTF8Char*)"boundary_stone"))
										{
											if (elemType == 0)
											{
												elemType = 59;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"information"))
									{
										if (vName->Equals((const UTF8Char*)"guidepost"))
										{
											if (elemType == 0)
											{
												elemType = 31;
											}
										}
										else if (vName->Equals((const UTF8Char*)"map"))
										{
											if (elemType == 0)
											{
												elemType = 32;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"landuse"))
									{
										if (vName->Equals((const UTF8Char*)"railway"))
										{
											if (elemType == 0)
											{
												elemType = 53;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"leisure"))
									{
										if (vName->Equals((const UTF8Char*)"marina"))
										{
											if (elemType == 0)
											{
												elemType = 61;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"man_made"))
									{
										if (vName->Equals((const UTF8Char*)"pier"))
										{
											if (elemType == 0)
											{
												elemType = 60;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"natural"))
									{
										if (vName->Equals((const UTF8Char*)"peak"))
										{
											if (elemType == 0)
											{
												elemType = 38;
											}
										}
										else if (vName->Equals((const UTF8Char*)"rock"))
										{
											if (elemType == 0)
											{
												elemType = 58;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"parking"))
									{
										if (vName->Equals((const UTF8Char*)"multi-storey"))
										{
											if (elemType == 0)
											{
												elemType = 44;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"place"))
									{
										if (vName->Equals((const UTF8Char*)"village"))
										{
											if (elemType == 0)
											{
												elemType = 39;
											}
										}
										else if (vName->Equals((const UTF8Char*)"locality"))
										{
											if (elemType == 0)
											{
												elemType = 43;
											}
										}
										else if (vName->Equals((const UTF8Char*)"suburb"))
										{
											if (elemType == 0)
											{
												elemType = 43;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"power"))
									{
										if (vName->Equals((const UTF8Char*)"generator"))
										{
											if (elemType == 0)
											{
												elemType = 54;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"railway"))
									{
										if (vName->Equals((const UTF8Char*)"station"))
										{
											if (elemType == 0)
											{
												elemType = 53;
											}
										}
										else if (vName->Equals((const UTF8Char*)"subway_entrance"))
										{
										}
									}
									else if (kName->Equals((const UTF8Char*)"religion"))
									{
										if (vName->Equals((const UTF8Char*)"taoist"))
										{
											if (elemType == 0)
											{
												elemType = 42;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"shop"))
									{
										if (vName->Equals((const UTF8Char*)"supermarket"))
										{
											if (elemType == 0)
											{
												elemType = 55;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"sport"))
									{
										if (vName->Equals((const UTF8Char*)"swimming"))
										{
											if (elemType == 0)
											{
												elemType = 62;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"tourism"))
									{
										if (vName->Equals((const UTF8Char*)"information"))
										{
										}
										else if (vName->Equals((const UTF8Char*)"viewpoint"))
										{
											if (elemType == 0)
											{
												elemType = 51;
											}
										}
										else if (vName->Equals((const UTF8Char*)"camp_site"))
										{
											if (elemType == 0)
											{
												elemType = 52;
											}
										}
									}
									else if (kName->Equals((const UTF8Char*)"traffic_calming"))
									{
										if (vName->Equals((const UTF8Char*)"rumble_strip"))
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
							else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
								NEW_CLASS(layers[elemType - 1], Map::VectorLayer(Map::DRAW_LAYER_POINT, fileName, colCnt, colName, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), 0, layerNames[elemType - 1]));
							}
							Math::Point *pt;
							NEW_CLASS(pt, Math::Point(4326, lon, lat));
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
			else if (reader->GetNodeText()->Equals((const UTF8Char*)"way"))
			{
				Int32 elemType = 0;
				Text::String *names[2] = {0, 0};
				Text::String *kName;
				Text::String *vName;
				Int64 nodeId;
				Bool isErr = false;
				Bool isPG = false;
				latList.Clear();
				lonList.Clear();
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (reader->GetNodeText()->Equals((const UTF8Char*)"nd"))
						{
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttrib(i);
								if (attr->name->Equals((const UTF8Char*)"ref"))
								{
									nodeId = attr->value->ToInt64();
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
						else if (reader->GetNodeText()->Equals((const UTF8Char*)"tag"))
						{
							kName = 0;
							vName = 0;
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttrib(i);
								if (attr->name->Equals((const UTF8Char*)"k"))
								{
									kName = attr->value;
								}
								else if (attr->name->Equals((const UTF8Char*)"v"))
								{
									vName = attr->value;
								}
							}
							if (kName && vName)
							{
								if (kName->Equals((const UTF8Char*)"name"))
								{
									SDEL_STRING(names[0]);
									names[0] = vName->Clone();
								}
								else if (kName->StartsWith((const UTF8Char*)"name:"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"oneway"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"bridge"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"addr:housenumber"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"building:levels"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"layer"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"psv"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"vehicle"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"bicycle"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"foot"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"surface"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"segregated"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"gauge"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"wikipedia"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"access"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"ref"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"source"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"parking"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"emergency"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"wheelchair"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"tunnel"))
								{
								}
								else if (kName->StartsWith((const UTF8Char*)"alt_name"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"man_made"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"sport"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"motor_vehicle"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"electrified"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"voltage"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"width"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"covered"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"website"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"religion"))
								{
								}
								else if (kName->Equals((const UTF8Char*)"amenity"))
								{
									if (vName->Equals((const UTF8Char*)"school"))
									{
										if (elemType == 0)
										{
											elemType = 6;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("School"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"parking"))
									{
										if (elemType == 0)
										{
											elemType = 7;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Parking"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"hospital"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Hospital"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"college"))
									{
										if (elemType == 0)
										{
											elemType = 6;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("College"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"place_of_worship"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("PlaceOfWorship"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"nursing_home"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("NursingHome"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"community_centre"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("CommunityCentre"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"university"))
									{
										if (elemType == 0)
										{
											elemType = 6;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("University"));
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"barrier"))
								{
									if (vName->Equals((const UTF8Char*)"wire_fence"))
									{
										if (elemType == 0)
										{
											elemType = 21;
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"building"))
								{
									if (vName->Equals((const UTF8Char*)"yes"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Building"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"house"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("House"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"school"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("School"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"apartments"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Apartments"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"residential"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Residential"));
										}
									}
									else
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Unknown"));
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"highway"))
								{
									if (vName->Equals((const UTF8Char*)"service"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Service"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"secondary"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Secondary"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"footway"))
									{
										if (elemType == 0)
										{
											elemType = 17;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Footway"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"motorway"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Motorway"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"motorway_link"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("MotorwayLink"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"residential"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Residential"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"tertiary"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Tertiary"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"path"))
									{
										if (elemType == 0)
										{
											elemType = 17;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Path"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"primary"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Primary"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"steps"))
									{
										if (elemType == 0)
										{
											elemType = 17;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Steps"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"cycleway"))
									{
										if (elemType == 0)
										{
											elemType = 19;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Cycleway"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"pedestrian"))
									{
										if (elemType == 0)
										{
											elemType = 17;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Pedestrian"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"track"))
									{
										if (elemType == 0)
										{
											elemType = 16;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Pedestrian"));
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"landuse"))
								{
									if (vName->Equals((const UTF8Char*)"reservoir"))
									{
										if (elemType == 0)
										{
											elemType = 10;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Reservoir"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"residential"))
									{
										if (elemType == 0)
										{
											elemType = 2;
											isPG = true;
										}
									}
									else if (vName->Equals((const UTF8Char*)"commercial"))
									{
										if (elemType == 0)
										{
											elemType = 3;
											isPG = true;
										}
									}
									else if (vName->Equals((const UTF8Char*)"industrial"))
									{
										if (elemType == 0)
										{
											elemType = 4;
											isPG = true;
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"leisure"))
								{
									if (vName->Equals((const UTF8Char*)"sports_centre"))
									{
										if (elemType == 0)
										{
											elemType = 5;
											isPG = true;
										}
									}
									else if (vName->Equals((const UTF8Char*)"park"))
									{
										if (elemType == 0)
										{
											elemType = 5;
											isPG = true;
										}
									}
									else if (vName->Equals((const UTF8Char*)"pitch"))
									{
										if (elemType == 0)
										{
											elemType = 5;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Pitch"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"swimming_pool"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("SwimmingPool"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"track"))
									{
										if (elemType == 0)
										{
											elemType = 5;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Track"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"stadium"))
									{
										if (elemType == 0)
										{
											elemType = 5;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Stadium"));
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"natural"))
								{
									if (vName->Equals((const UTF8Char*)"coastline"))
									{
										if (elemType == 0)
										{
											elemType = 1;
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"power"))
								{
									if (vName->Equals((const UTF8Char*)"sub_station"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("PowerSubStation"));
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"railway"))
								{
									if (vName->Equals((const UTF8Char*)"rail"))
									{
										if (elemType == 0)
										{
											elemType = 15;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Rail"));
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"shop"))
								{
									if (vName->Equals((const UTF8Char*)"garden_centre"))
									{
										if (elemType == 0)
										{
											elemType = 22;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("GardenCentre"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"mall"))
									{
										if (elemType == 0)
										{
											elemType = 18;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Mall"));
										}
									}
								}
								else if (kName->Equals((const UTF8Char*)"waterway"))
								{
									if (vName->Equals((const UTF8Char*)"stream"))
									{
										if (elemType == 0)
										{
											elemType = 11;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Stream"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"weir"))
									{
										if (elemType == 0)
										{
											elemType = 0;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Weir"));
										}
									}
									else if (vName->Equals((const UTF8Char*)"riverbank"))
									{
										if (elemType == 0)
										{
											elemType = 8;
											isPG = true;
											SDEL_STRING(names[1]);
											names[1] = Text::String::New(UTF8STRC("Riverbank"));
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
							NEW_CLASS(layers[elemType - 1], Map::VectorLayer(Map::DRAW_LAYER_POLYGON, fileName, colCnt, pgName, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), 0, layerNames[elemType - 1]));
						}
						Math::Polygon *pg;
						NEW_CLASS(pg, Math::Polygon(4326, 1, latList.GetCount()));
						Double *points = pg->GetPointList(&i);
						while (i-- > 0)
						{
							points[i * 2 + 0] = lonList.GetItem(i);
							points[i * 2 + 1] = latList.GetItem(i);
						}
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
							NEW_CLASS(layers[elemType - 1], Map::VectorLayer(Map::DRAW_LAYER_POLYLINE, fileName, colCnt, pgName, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), 0, layerNames[elemType - 1]));
						}
						Math::Polyline *pl;
						NEW_CLASS(pl, Math::Polyline(4326, 1, latList.GetCount()));
						Double *points = pl->GetPointList(&i);
						while (i-- > 0)
						{
							points[i * 2 + 0] = lonList.GetItem(i);
							points[i * 2 + 1] = latList.GetItem(i);
						}
						layers[elemType - 1]->AddVector(pl, names);
					}
				}
				SDEL_STRING(names[0]);
				SDEL_STRING(names[1]);
			}
			else if (reader->GetNodeText()->Equals((const UTF8Char*)"relation"))
			{
				reader->SkipElement();
			}
			else
			{
				reader->SkipElement();
			}
		}
	}
	Data::ArrayList<OSMNodeInfo*> *nodeList;
	nodeList = nodeMap.GetValues();
	i = nodeList->GetCount();
	while (i-- > 0)
	{
		MemFree(nodeList->GetItem(i));
	}
	Map::MapLayerCollection *layerList;
	NEW_CLASS(layerList, Map::MapLayerCollection(fileName, (const UTF8Char*)"OSM"));
	i = 0;
	while (i < OSMTYPECNT)
	{
		if (layers[i])
		{
			layerList->Add(layers[i]);
		}
		i++;
	}
	return layerList;
}
