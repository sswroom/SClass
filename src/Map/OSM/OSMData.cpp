#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/Sort/ArtificialQuickSort_C.h"
#include "DB/ColDef.h"
#include "Map/OSM/OSMData.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/Point.h"

OSInt Map::OSM::ElementComparator::Compare(NN<ElementInfo> a, NN<ElementInfo> b) const
{
	if (a->type > b->type)
	{
		return 1;
	}
	else if (a->type < b->type)
	{
		return -1;
	}
	else
	{
		if (a->id > b->id)
		{
			return 1;
		}
		else if (a->id < b->id)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

void __stdcall Map::OSM::OSMData::FreeRelationMember(NN<RelationMember> member)
{
	OPTSTR_DEL(member->role);
	member.Delete();
}

void __stdcall Map::OSM::OSMData::FreeElement(NN<ElementInfo> elem)
{
	NN<Data::ArrayListNN<TagInfo>> tags;
	if (elem->tags.SetTo(tags))
	{
		NN<TagInfo> tag;
		UOSInt i = tags->GetCount();
		while (i-- > 0)
		{
			tag = tags->GetItemNoCheck(i);
			tag->k->Release();
			tag->v->Release();
			tag.Delete();
		}
		tags.Delete();
	}
	if (elem->type == ElementType::Node)
	{
	}
	else if (elem->type == ElementType::Way)
	{
		NN<WayInfo> way = NN<WayInfo>::ConvertFrom(elem);
		way->nodes.Clear();
	}
	else if (elem->type == ElementType::Relation)
	{
		NN<RelationInfo> rel = NN<RelationInfo>::ConvertFrom(elem);
		rel->members.FreeAll(FreeRelationMember);
	}
	elem.Delete();
}

void Map::OSM::OSMData::SortElements()
{
	ElementComparator cmp;
	Data::Sort::ArtificialQuickSort::Sort<NN<ElementInfo>>(this->elements, cmp);
	this->nodeSorted = true;
	this->waySorted = true;
	this->relationSorted = true;
}

Map::OSM::LayerType Map::OSM::OSMData::CalcElementLayerType(NN<ElementInfo> elem) const
{
	NN<Data::ArrayListNN<TagInfo>> tags;
	if (!elem->tags.SetTo(tags))
	{
		return LayerType::Unknown;
	}
	NN<TagInfo> tag;
	UOSInt i = 0;
	UOSInt j = tags->GetCount();
	while (i < j)
	{
		tag = tags->GetItemNoCheck(i);
		if (tag->k->Equals(UTF8STRC("aeroway")))
		{
			return LayerType::Aeroway;
		}
		else if (tag->k->Equals(UTF8STRC("amenity")))
		{
			if (tag->v->Equals(UTF8STRC("fast_food")))
			{
				return LayerType::AmenityFastFood;
			}
			else if (tag->v->Equals(UTF8STRC("kindergarten")))
			{
				return LayerType::AmenityKindergarten;
			}
			else if (tag->v->Equals(UTF8STRC("marketplace")))
			{
				return LayerType::AmenityMarketplace;
			}
			else if (tag->v->Equals(UTF8STRC("police")))
			{
				return LayerType::AmenityPolice;
			}
			else if (tag->v->Equals(UTF8STRC("post_box")))
			{
				return LayerType::AmenityPostBox;
			}
			else if (tag->v->Equals(UTF8STRC("restaurant")))
			{
				return LayerType::AmenityRestaurant;
			}
			else if (tag->v->Equals(UTF8STRC("telephone")))
			{
				return LayerType::AmenityTelephone;
			}
			else
			{
				return LayerType::Amenity;
			}
		}
		else if (tag->k->Equals(UTF8STRC("boundary")))
		{
			return LayerType::Boundary;
		}
		else if (tag->k->Equals(UTF8STRC("building")))
		{
			return LayerType::Building;
		}
		else if (tag->k->Equals(UTF8STRC("golf")))
		{
			if (tag->v->Equals(UTF8STRC("bunker")))
			{
				return LayerType::GolfBunker;
			}
			else if (tag->v->Equals(UTF8STRC("fairway")))
			{
				return LayerType::GolfFairway;
			}
			else if (tag->v->Equals(UTF8STRC("hole")))
			{
				return LayerType::GolfHole;
			}
			else if (tag->v->Equals(UTF8STRC("rough")))
			{
				return LayerType::GolfRough;
			}
			else
			{
				return LayerType::Golf;
			}
		}
		else if (tag->k->Equals(UTF8STRC("highway")))
		{
			if (tag->v->Equals(UTF8STRC("bus_stop")))
			{
				return LayerType::HighwayBusStop;
			}
			else if (tag->v->Equals(UTF8STRC("construction")))
			{
				return LayerType::HighwayConstruction;
			}
			else if (tag->v->Equals(UTF8STRC("corridor")))
			{
				return LayerType::HighwayCorridor;
			}
			else if (tag->v->Equals(UTF8STRC("crossing")))
			{
				return LayerType::HighwayCrossing;
			}
			else if (tag->v->Equals(UTF8STRC("cycleway")))
			{
				return LayerType::HighwayCycleway;
			}
			else if (tag->v->Equals(UTF8STRC("elevator")))
			{
				return LayerType::HighwayElevator;
			}
			else if (tag->v->Equals(UTF8STRC("emergency_access_point")))
			{
				return LayerType::HighwayEmergencyAccessPoint;
			}
			else if (tag->v->Equals(UTF8STRC("footway")))
			{
				return LayerType::HighwayFootway;
			}
			else if (tag->v->Equals(UTF8STRC("give_way")))
			{
				return LayerType::HighwayGiveWay;
			}
			else if (tag->v->Equals(UTF8STRC("milestone")))
			{
				return LayerType::HighwayMilestone;
			}
			else if (tag->v->Equals(UTF8STRC("motorway")))
			{
				return LayerType::HighwayMotorway;
			}
			else if (tag->v->Equals(UTF8STRC("motorway_link")))
			{
				return LayerType::HighwayMotorwayLink;
			}
			else if (tag->v->Equals(UTF8STRC("path")))
			{
				return LayerType::HighwayPath;
			}
			else if (tag->v->Equals(UTF8STRC("pedestrian")))
			{
				return LayerType::HighwayPedestrian;
			}
			else if (tag->v->Equals(UTF8STRC("platform")))
			{
				return LayerType::HighwayPlatform;
			}
			else if (tag->v->Equals(UTF8STRC("primary")))
			{
				return LayerType::HighwayPrimary;
			}
			else if (tag->v->Equals(UTF8STRC("primary_link")))
			{
				return LayerType::HighwayPrimaryLink;
			}
			else if (tag->v->Equals(UTF8STRC("residential")))
			{
				return LayerType::HighwayResidential;
			}
			else if (tag->v->Equals(UTF8STRC("rest_area")))
			{
				return LayerType::HighwayRestArea;
			}
			else if (tag->v->Equals(UTF8STRC("secondary")))
			{
				return LayerType::HighwaySecondary;
			}
			else if (tag->v->Equals(UTF8STRC("secondary_link")))
			{
				return LayerType::HighwaySecondaryLink;
			}
			else if (tag->v->Equals(UTF8STRC("service")))
			{
				return LayerType::HighwayService;
			}
			else if (tag->v->Equals(UTF8STRC("speed_camera")))
			{
				return LayerType::HighwaySpeedCamera;
			}
			else if (tag->v->Equals(UTF8STRC("steps")))
			{
				return LayerType::HighwaySteps;
			}
			else if (tag->v->Equals(UTF8STRC("street_lamp")))
			{
				return LayerType::HighwayStreetLamp;
			}
			else if (tag->v->Equals(UTF8STRC("tertiary")))
			{
				return LayerType::HighwayTertiary;
			}
			else if (tag->v->Equals(UTF8STRC("traffic_signals")))
			{
				return LayerType::HighwayTrafficSignals;
			}
			else if (tag->v->Equals(UTF8STRC("trunk")))
			{
				return LayerType::HighwayTrunk;
			}
			else if (tag->v->Equals(UTF8STRC("trunk_link")))
			{
				return LayerType::HighwayTrunkLink;
			}
			else if (tag->v->Equals(UTF8STRC("unclassified")))
			{
				return LayerType::HighwayUnclassified;
			}
			else
			{
				return LayerType::Highway;
			}
		}
		else if (tag->k->Equals(UTF8STRC("landuse")))
		{
			if (tag->v->Equals(UTF8STRC("industrial")))
			{
				return LayerType::LanduseIndustrial;
			}
			else if (tag->v->Equals(UTF8STRC("residential")))
			{
				return LayerType::LanduseResidential;
			}
			else
			{
				return LayerType::Landuse;
			}
		}
		else if (tag->k->Equals(UTF8STRC("leisure")))
		{
			if (tag->v->Equals(UTF8STRC("park")))
			{
				return LayerType::LeisurePark;
			}
			else if (tag->v->Equals(UTF8STRC("pitch")))
			{
				return LayerType::LeisurePitch;
			}
			else
			{
				return LayerType::Leisure;
			}
		}
		else if (tag->k->Equals(UTF8STRC("natural")))
		{
			if (tag->v->Equals(UTF8STRC("bay")))
			{
				return LayerType::NaturalBay;
			}
			else if (tag->v->Equals(UTF8STRC("cape")))
			{
				return LayerType::NaturalCape;
			}
			else if (tag->v->Equals(UTF8STRC("grassland")))
			{
				return LayerType::NaturalGrassland;
			}
			else if (tag->v->Equals(UTF8STRC("peak")))
			{
				return LayerType::NaturalPeak;
			}
			else if (tag->v->Equals(UTF8STRC("ridge")))
			{
				return LayerType::NaturalRidge;
			}
			else if (tag->v->Equals(UTF8STRC("rock")))
			{
				return LayerType::NaturalRock;
			}
			else if (tag->v->Equals(UTF8STRC("saddle")))
			{
				return LayerType::NaturalSaddle;
			}
			else if (tag->v->Equals(UTF8STRC("scrub")))
			{
				return LayerType::NaturalScrub;
			}
			else if (tag->v->Equals(UTF8STRC("valley")))
			{
				return LayerType::NaturalValley;
			}
			else if (tag->v->Equals(UTF8STRC("wood")))
			{
				return LayerType::NaturalWood;
			}
			else
			{
				return LayerType::Natural;
			}
		}
		else if (tag->k->Equals(UTF8STRC("place")))
		{
			if (tag->v->Equals(UTF8STRC("hamlet")))
			{
				return LayerType::PlaceHamlet;
			}
			else if (tag->v->Equals(UTF8STRC("locality")))
			{
				return LayerType::PlaceLocality;
			}
			else
			{
				return LayerType::Place;
			}
		}
		else if (tag->k->Equals(UTF8STRC("power")))
		{
			if (tag->v->Equals(UTF8STRC("line")))
			{
				return LayerType::PowerLine;
			}
			else if (tag->v->Equals(UTF8STRC("substation")))
			{
				return LayerType::PowerSubstation;
			}
			else
			{
				return LayerType::Power;
			}
		}
		else if (tag->k->Equals(UTF8STRC("railway")))
		{
			return LayerType::Railway;
		}
		else if (tag->k->Equals(UTF8STRC("transport")))
		{
			return LayerType::Transport;
		}
		else if (tag->k->Equals(UTF8STRC("waterway")))
		{
			if (tag->v->Equals(UTF8STRC("stream")))
			{
				return LayerType::WaterwayStream;
			}
			else
			{
				return LayerType::Waterway;
			}
		}
		i++;
	}
	return LayerType::Unknown;
}

Map::OSM::OSMData::OSMData(Text::CStringNN sourceName) : Map::MapDrawLayer(sourceName, 0, CSTR("OSM"), Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->groupDist = 0.01;
	this->lang = nullptr;
	this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(0, 0));
	this->dataBounds = Math::RectAreaDbl(Math::Coord2DDbl(-180, -90), Math::Coord2DDbl(180, 90));
	this->note = nullptr;
	this->osmBaseTime = nullptr;
	this->nodeSorted = true;
	this->waySorted = true;
	this->relationSorted = true;
	this->showUnknown = false;
	this->currScale = 1000;
	this->mixedData = MixedData::AllData;
	this->SetStyleDefault();
}

Map::OSM::OSMData::~OSMData()
{
	OPTSTR_DEL(this->lang);
	OPTSTR_DEL(this->note);
	this->eleGroups.DeleteAll();
	this->elements.FreeAll(Map::OSM::OSMData::FreeElement);
}

void Map::OSM::OSMData::SetCurrScale(Double scale)	
{
	this->currScale = scale;
}

NN<Map::OSM::NodeInfo> Map::OSM::OSMData::NewNode(Int64 id, Double lat, Double lon)
{
	Int32 x;
	Int32 y;
	NN<NodeInfo> node;
	NEW_CLASSNN(node, NodeInfo());
	node->id = id;
	node->type = ElementType::Node;
	node->hasParent = false;
	node->lat = lat;
	node->lon = lon;
	this->elements.Add(node);
	this->nodeSorted = false;

	x = (Int32)(lon / this->groupDist);
	y = (Int32)(lat / this->groupDist);
	NN<ElementGroup> grp;
	if (!this->eleGroups.Get(x << 16 | y).SetTo(grp))
	{
		NEW_CLASSNN(grp, ElementGroup());
		grp->x = x;
		grp->y = y;
		this->eleGroups.Put(x << 16 | y, grp);
	}
	grp->elements.Add(node);

	if (this->bounds.IsZero())
	{
		this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(lon, lat), Math::Coord2DDbl(lon, lat));
	}
	else
	{
		this->bounds = this->bounds.MergePoint(Math::Coord2DDbl(lon, lat));
	}	
	return node;
}

NN<Map::OSM::WayInfo> Map::OSM::OSMData::NewWay(Int64 id)
{
	NN<WayInfo> way;
	NEW_CLASSNN(way, WayInfo());
	way->id = id;
	way->type = ElementType::Way;
	way->hasParent = false;
	way->bounds = Math::RectAreaDbl(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(0, 0));
	this->elements.Add(way);
	return way;
}

NN<Map::OSM::RelationInfo> Map::OSM::OSMData::NewRelation(Int64 id)
{
	NN<RelationInfo> rel;
	NEW_CLASSNN(rel, RelationInfo());
	rel->id = id;
	rel->type = ElementType::Relation;
	rel->hasParent = false;
	this->elements.Add(rel);
	return rel;
}

Optional<Map::OSM::ElementInfo> Map::OSM::OSMData::GetElementById(Int64 id, ElementType type)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->elements.GetCount() - 1;
	while (i <= j)
	{
		OSInt k = (i + j) >> 1;
		NN<ElementInfo> elem = this->elements.GetItemNoCheck((UOSInt)k);
		if (elem->type < type || (elem->type == type && elem->id < id))
		{
			i = k + 1;
		}
		else if (elem->type > type || (elem->type == type && elem->id > id))
		{
			j = k - 1;
		}
		else
		{
			return elem;
		}
	}
	return nullptr;
}

Optional<Map::OSM::NodeInfo> Map::OSM::OSMData::GetNodeById(Int64 id)
{
	NN<ElementInfo> elem;
	if (this->nodeSorted == false)
	{
		this->SortElements();
	}
	if (this->GetElementById(id, ElementType::Node).SetTo(elem))
	{
		return NN<NodeInfo>::ConvertFrom(elem);
	}
	return nullptr;
}

Optional<Map::OSM::NodeInfo> Map::OSM::OSMData::GetNodeByPos(Double lat, Double lon)
{
	Int32 x = (Int32)(lon / this->groupDist);
	Int32 y = (Int32)(lat / this->groupDist);
	NN<ElementGroup> grp;
	if (this->eleGroups.Get(x << 16 | y).SetTo(grp))
	{
		UOSInt i = 0;
		UOSInt j = grp->elements.GetCount();
		while (i < j)
		{
			NN<ElementInfo> elem = grp->elements.GetItemNoCheck(i);
			if (elem->type == ElementType::Node)
			{
				NN<NodeInfo> node = NN<NodeInfo>::ConvertFrom(elem);
				if (node->lat == lat && node->lon == lon)
				{
					return node;
				}
			}
			i++;
		}
	}
	return nullptr;
}

Optional<Map::OSM::WayInfo> Map::OSM::OSMData::GetWayById(Int64 id)
{
	NN<ElementInfo> elem;
	if (this->waySorted == false)
	{
		this->SortElements();
	}
	if (this->GetElementById(id, ElementType::Way).SetTo(elem))
	{
		return NN<WayInfo>::ConvertFrom(elem);
	}
	return nullptr;
}

Optional<Map::OSM::RelationInfo> Map::OSM::OSMData::GetRelationById(Int64 id)
{
	NN<ElementInfo> elem;
	if (this->relationSorted == false)
	{
		this->SortElements();
	}
	if (this->GetElementById(id, ElementType::Relation).SetTo(elem))
	{
		return NN<RelationInfo>::ConvertFrom(elem);
	}
	return nullptr;
}

void Map::OSM::OSMData::WayAddNode(NN<WayInfo> way, NN<NodeInfo> node)
{
	way->nodes.Add(node);
	node->hasParent = true;
	if (way->bounds.IsZero())
	{
		way->bounds = Math::RectAreaDbl(Math::Coord2DDbl(node->lon, node->lat), Math::Coord2DDbl(node->lon, node->lat));
	}
	else
	{
		way->bounds = way->bounds.MergePoint(Math::Coord2DDbl(node->lon, node->lat));
	}
	Int32 x = (Int32)(node->lon / this->groupDist);
	Int32 y = (Int32)(node->lat / this->groupDist);
	NN<ElementGroup> grp;
	if (!this->eleGroups.Get(x << 16 | y).SetTo(grp))
	{
		NEW_CLASSNN(grp, ElementGroup());
		grp->x = x;
		grp->y = y;
		this->eleGroups.Put(x << 16 | y, grp);
	}
	if (grp->elements.GetCount() == 0 || grp->elements.GetItemNoCheck(grp->elements.GetCount() - 1) != way)
	{
		grp->elements.Add(way);
	}
}

void Map::OSM::OSMData::ElementAddTag(NN<ElementInfo> elem, NN<Text::String> k, NN<Text::String> v)
{
	NN<Data::ArrayListNN<TagInfo>> tags;
	if (!elem->tags.SetTo(tags))
	{
		NEW_CLASSNN(tags, Data::ArrayListNN<TagInfo>());
		elem->tags = tags;
	}
	NN<TagInfo> tag;
	NEW_CLASSNN(tag, TagInfo());
	tag->k = k->Clone();
	tag->v = v->Clone();
	tags->Add(tag);
}

void Map::OSM::OSMData::RelationAddMember(NN<RelationInfo> rel, ElementType type, Int64 refId, Optional<Text::String> role)
{
	NN<RelationMember> member;
	NEW_CLASSNN(member, RelationMember());
	member->type = type;
	member->refId = refId;
	member->role = Text::String::CopyOrNull(role);
	rel->members.Add(member);
}

void Map::OSM::OSMData::SetLang(Text::CString lang)
{
	OPTSTR_DEL(this->lang);
	this->lang = Text::String::NewOrNull(lang);
}

void Map::OSM::OSMData::SetDataBounds(Math::RectAreaDbl bounds)
{
	this->dataBounds = bounds;
}

Math::RectAreaDbl Map::OSM::OSMData::GetDataBounds() const
{
	return this->dataBounds;
}

void Map::OSM::OSMData::SetGroupDist(Double groupDist)
{
	if (groupDist <= 0)
		return;
	this->groupDist = groupDist;
}

void Map::OSM::OSMData::SetOSMBase(Data::Timestamp baseTime)
{
	this->osmBaseTime = baseTime;
}

void Map::OSM::OSMData::SetNote(Text::CStringNN note)
{
	OPTSTR_DEL(this->note);
	this->note = Text::String::New(note);
}

NN<Math::Geometry::Vector2D> Map::OSM::OSMData::CreateVector(NN<ElementInfo> elem)
{
	NN<Math::Geometry::Vector2D> ret;
	LayerType layerType = elem->layerType;
	if (elem->type == ElementType::Node)
	{
		NN<NodeInfo> node = NN<NodeInfo>::ConvertFrom(elem);
		NEW_CLASSNN(ret, Math::Geometry::Point(4326, node->lon, node->lat));
		return ret;
	}
	else if (elem->type == ElementType::Way)
	{
		NN<WayInfo> way = NN<WayInfo>::ConvertFrom(elem);
		UOSInt i = 0;
		UOSInt j = way->nodes.GetCount();
		if (j < 2)
		{
			printf("OSMData: Way %lld has no enough nodes\r\n", way->id);
			if (j == 1)
			{
				NN<NodeInfo> node = way->nodes.GetItemNoCheck(0);
				NEW_CLASSNN(ret, Math::Geometry::Point(4326, node->lon, node->lat));
				return ret;
			}
			NEW_CLASSNN(ret, Math::Geometry::Point(4326, 0, 0));
			return ret;
		}
		if (this->layerSpecs[(UOSInt)layerType].isArea)
		{
			NN<Math::Geometry::Polygon> pg;
			NN<Math::Geometry::LinearRing> line;
			UnsafeArray<Math::Coord2DDbl> coords;
			NEW_CLASSNN(line, Math::Geometry::LinearRing(4326, j, false, false));
			coords = line->GetPointList(j);
			while (i < j)
			{
				NN<NodeInfo> node = way->nodes.GetItemNoCheck(i);
				coords[i].x = node->lon;
				coords[i].y = node->lat;
				i++;
			}
			// Landuse: e0dfdf
			NEW_CLASSNN(pg, Math::Geometry::Polygon(4326));
			pg->AddGeometry(line);
			return pg;
		}
		else
		{
			NN<Math::Geometry::LineString> line;
			UnsafeArray<Math::Coord2DDbl> coords;
			NEW_CLASSNN(line, Math::Geometry::LineString(4326, j, false, false));
			coords = line->GetPointList(j);
			while (i < j)
			{
				NN<NodeInfo> node = way->nodes.GetItemNoCheck(i);
				coords[i].x = node->lon;
				coords[i].y = node->lat;
				i++;
			}
			return line;
		}
	}
	else if (elem->type == ElementType::Relation)
	{
		NN<RelationInfo> rel = NN<RelationInfo>::ConvertFrom(elem);
		if (rel->members.GetCount() == 0)
		{
			printf("OSMData: Relation %lld has no members\r\n", rel->id);
			NEW_CLASSNN(ret, Math::Geometry::Point(4326, 0, 0));
			return ret;
		}
		NN<RelationMember> firstMember = rel->members.GetItemNoCheck(0);
		if (!this->waySorted)
		{
			this->SortElements();
		}
		if (this->GetElementById(firstMember->refId, firstMember->type).SetTo(elem))
		{
			return CreateVector(elem);
		}
		else
		{
			printf("OSMData: Relation %lld member %lld not found\r\n", rel->id, rel->members.GetItemNoCheck(0)->refId);
			NEW_CLASSNN(ret, Math::Geometry::Point(4326, 0, 0));
			return ret;
		}
	}
	else
	{
		printf("OSMData: Unknown element type: %d\r\n", (Int32)elem->type);
		NEW_CLASSNN(ret, Math::Geometry::Point(4326, 0, 0));
		return ret;
	}
}

void Map::OSM::OSMData::SetShowUnknown(Bool showUnknown)
{
	this->showUnknown = showUnknown;
}

Bool Map::OSM::OSMData::IsShowUnknown() const
{
	return this->showUnknown;
}

void Map::OSM::OSMData::BuildIndex()
{
	this->SortElements();
	UOSInt i = 0;
	UOSInt j = this->elements.GetCount();
	NN<ElementInfo> elem;
	while (i < j)
	{
		elem = this->elements.GetItemNoCheck(i);
		elem->layerType = this->CalcElementLayerType(elem);
		i++;
	}
}

void Map::OSM::OSMData::SetStyleDefault()
{
	UOSInt i = (UOSInt)LayerType::Count;
	while (i-- > 0)
	{
		this->layerSpecs[i].minScale = 1;
		this->layerSpecs[i].maxScale = 50000.0;
		this->layerSpecs[i].isArea = false;
		this->layerSpecs[i].hide = false;
	}
	this->layerSpecs[(UOSInt)LayerType::AmenityFastFood].maxScale = 10000.0;
	this->layerSpecs[(UOSInt)LayerType::AmenityMarketplace].maxScale = 10000.0;
	this->layerSpecs[(UOSInt)LayerType::AmenityPostBox].maxScale = 5000.0;
	this->layerSpecs[(UOSInt)LayerType::AmenityRestaurant].maxScale = 10000.0;
	this->layerSpecs[(UOSInt)LayerType::AmenityTelephone].maxScale = 10000.0;
	this->layerSpecs[(UOSInt)LayerType::Building].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::HighwayBusStop].maxScale = 20000.0;
	this->layerSpecs[(UOSInt)LayerType::HighwayCycleway].maxScale = 100000.0;
	this->layerSpecs[(UOSInt)LayerType::HighwayFootway].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::HighwayPath].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::HighwayTrafficSignals].maxScale = 10000.0;
	this->layerSpecs[(UOSInt)LayerType::Landuse].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::LanduseIndustrial].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::LanduseResidential].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalBay].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalCape].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalGrassland].maxScale = 200000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalPeak].maxScale = 500000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalRock].maxScale = 10000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalSaddle].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalScrub].maxScale = 200000.0;
	this->layerSpecs[(UOSInt)LayerType::NaturalValley].maxScale = 10000.0;
	this->layerSpecs[(UOSInt)LayerType::PlaceHamlet].minScale = 5000.0;
	this->layerSpecs[(UOSInt)LayerType::PlaceHamlet].maxScale = 25000.0;
	this->layerSpecs[(UOSInt)LayerType::PlaceLocality].maxScale = 20000.0;
	this->layerSpecs[(UOSInt)LayerType::PowerLine].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::PowerSubstation].maxScale = 50000.0;
	this->layerSpecs[(UOSInt)LayerType::WaterwayStream].maxScale = 25000.0;
	this->layerSpecs[(UOSInt)LayerType::Unknown].maxScale = 2000000000.0;
	this->layerSpecs[(UOSInt)LayerType::AmenityPolice].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::AmenityKindergarten].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::Building].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::GolfBunker].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::GolfFairway].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::GolfRough].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::Landuse].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::LanduseIndustrial].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::LanduseResidential].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::LeisurePark].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::LeisurePitch].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::NaturalGrassland].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::NaturalScrub].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::NaturalWood].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::PowerSubstation].isArea = true;
	this->layerSpecs[(UOSInt)LayerType::Unknown].hide = true;
	this->layerSpecs[(UOSInt)LayerType::HighwayCrossing].hide = true;
	this->layerSpecs[(UOSInt)LayerType::HighwayEmergencyAccessPoint].hide = true;
	this->layerSpecs[(UOSInt)LayerType::HighwayGiveWay].hide = true;
	this->layerSpecs[(UOSInt)LayerType::HighwayMilestone].hide = true;
	this->layerSpecs[(UOSInt)LayerType::HighwayRestArea].hide = true;
	this->layerSpecs[(UOSInt)LayerType::HighwaySpeedCamera].hide = true;
	this->layerSpecs[(UOSInt)LayerType::HighwayStreetLamp].hide = true;
	this->layerSpecs[(UOSInt)LayerType::NaturalValley].hide = true;
}

void Map::OSM::OSMData::SetStyleCenterline()
{
	UOSInt i = (UOSInt)LayerType::Count;
	while (i-- > 0)
	{
		this->layerSpecs[i].minScale = 1;
		this->layerSpecs[i].maxScale = 50000.0;
		this->layerSpecs[i].isArea = false;
		this->layerSpecs[i].hide = true;
	}
	this->layerSpecs[(UOSInt)LayerType::HighwayMotorway].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayMotorwayLink].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayPrimary].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayPrimaryLink].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayResidential].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwaySecondary].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwaySecondaryLink].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayService].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayTertiary].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayTrunk].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayTrunkLink].hide = false;
	this->layerSpecs[(UOSInt)LayerType::HighwayUnclassified].hide = false;
}

UOSInt Map::OSM::OSMData::GetRelations(NN<Data::ArrayListNN<RelationInfo>> outArr) const
{
	UOSInt i = 0;
	UOSInt j = this->elements.GetCount();
	UOSInt ret = 0;
	while (i < j)
	{
		NN<ElementInfo> elem = this->elements.GetItemNoCheck(i);
		if (elem->type == ElementType::Relation)
		{
			outArr->Add(NN<RelationInfo>::ConvertFrom(elem));
			ret++;
		}
		i++;
	}
	return ret;
}

Map::DrawLayerType Map::OSM::OSMData::GetLayerType() const
{
	return Map::DrawLayerType::DRAW_LAYER_MIXED;
}

void Map::OSM::OSMData::SetMixedData(MixedData mixedData)
{
	this->mixedData = mixedData;
}

UOSInt Map::OSM::OSMData::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	NN<ElementInfo> elem;
	UOSInt i = 0;
	UOSInt j = this->elements.GetCount();
	while (i < j)
	{
		elem = this->elements.GetItemNoCheck(i);
		if (this->mixedData == MixedData::PointOnly && elem->type != ElementType::Node)
		{
		}
		else if (this->mixedData == MixedData::NonPointOnly && elem->type == ElementType::Node)
		{
		}
		else
		{
			if (this->showUnknown)
			{
				outArr->Add(elem->id << 2 | (UInt8)elem->type);
			}
			else if (!elem->hasParent)
			{
				LayerType layerType = elem->layerType;
				if (!this->layerSpecs[(UOSInt)layerType].hide)
				{
					outArr->Add(elem->id << 2 | (UInt8)elem->type);
				}
			}
		}		
		i++;
	}
	return j;
}

UOSInt Map::OSM::OSMData::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::OSM::OSMData::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Int32 minX = (Int32)(rect.min.x / this->groupDist);
	Int32 maxX = (Int32)(rect.max.x / this->groupDist);
	Int32 minY = (Int32)(rect.min.y / this->groupDist);
	Int32 maxY = (Int32)(rect.max.y / this->groupDist);
	OSInt si = this->eleGroups.GetIndex((minX << 16) | minY);
	UOSInt index = si >= 0 ? (UOSInt)si : (UOSInt)(~si);
	si = this->eleGroups.GetIndex((maxX << 16) | maxY);
	UOSInt endIndex = si >= 0 ? (UOSInt)si : (UOSInt)(~si);
	UOSInt ret = 0;
	NN<ElementInfo> elem;
	NN<ElementGroup> grp;
	Data::ArrayListInt64 tmpArr;
	while (index <= endIndex)
	{
		if (this->eleGroups.GetItem(index).SetTo(grp))
		{
			if (grp->x >= minX && grp->x <= maxX && grp->y >= minY && grp->y <= maxY)
			{
				UOSInt k = 0;
				UOSInt l = grp->elements.GetCount();
				while (k < l)
				{
					elem = grp->elements.GetItemNoCheck(k);
					if (!elem->hasParent)
					{
						if (this->mixedData == MixedData::PointOnly && elem->type != ElementType::Node)
						{
						}
						else if (this->mixedData == MixedData::NonPointOnly && elem->type == ElementType::Node)
						{
						}
						else
						{
							LayerType layerType = elem->layerType;
							Bool show = true;
							if (!this->showUnknown && this->layerSpecs[(UOSInt)layerType].hide)
							{
								show = false;
							}
							else
							{
								show = this->currScale >= this->layerSpecs[(UOSInt)layerType].minScale && this->currScale <= this->layerSpecs[(UOSInt)layerType].maxScale;
							}
							if (show)
							{
								tmpArr.Add(elem->id << 2 | (UInt8)elem->type);
							}
						}
					}
					k++;
				}
			}
		}
		index++;
	}
	ArtificialQuickSort_SortInt64(tmpArr.Arr().Ptr(), 0, (OSInt)tmpArr.GetCount() - 1);
	Int64 lastId = -1;
	UOSInt i = 0;
	UOSInt j = tmpArr.GetCount();
	while (i < j)
	{
		Int64 currId = tmpArr.GetItem(i);
		if (currId != lastId)
		{
			outArr->Add(currId);
			ret++;
			lastId = currId;
		}
		i++;
	}
	return ret;
}

Int64 Map::OSM::OSMData::GetObjectIdMax() const
{
	NN<ElementInfo> elem;
	elem = this->elements.GetItemNoCheck(this->elements.GetCount() - 1);
	return elem->id << 2 | (UInt8)elem->type;
}

UOSInt Map::OSM::OSMData::GetRecordCnt() const
{
	return this->elements.GetCount();
}

void Map::OSM::OSMData::ReleaseNameArr(Optional<NameArray> nameArr)
{
}

Bool Map::OSM::OSMData::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	NN<ElementInfo> elem;
	NN<Data::ArrayListNN<TagInfo>> tags;
	NN<TagInfo> tag;
	NN<Text::String> lang;
	NN<Math::Geometry::Vector2D> vec;
	UOSInt i;
	UOSInt j;
	if (!this->GetElementById(id >> 2, (ElementType)(id & 3)).SetTo(elem))
	{
		return false;
	}
	switch (strIndex)
	{
	case 0:
		sb->AppendI64(id);
		return true;
	case 1:
		if (!elem->tags.SetTo(tags))
		{
			return false;
		}
		if (this->lang.SetTo(lang))
		{
			i = 0;
			j = tags->GetCount();
			while (i < j)
			{
				tag = tags->GetItemNoCheck(i);
				if (tag->k->StartsWith(UTF8STRC("name:")) && tag->k->ToCString().Substring(5).Equals(lang->ToCString()))
				{
					sb->Append(tag->v);
					return true;
				}
				i++;
			}
		}
		i = 0;
		j = tags->GetCount();
		while (i < j)
		{
			tag = tags->GetItemNoCheck(i);
			if (tag->k->Equals(UTF8STRC("name")))
			{
				sb->Append(tag->v);
				return true;
			}
			i++;
		}
		return false;
	case 2:
		return false;
	case 3:
		vec = CreateVector(elem);
		{
			Math::WKTWriter wktWriter;
			return wktWriter.ToText(sb, vec);
		}
	}
	return false;
}

UOSInt Map::OSM::OSMData::GetColumnCnt() const
{
	return 4;
}

UnsafeArrayOpt<UTF8Char> Map::OSM::OSMData::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const
{
	switch (colIndex)
	{
	case 0:
		return Text::StrConcatC(buff, UTF8STRC("id"));
	case 1:
		return Text::StrConcatC(buff, UTF8STRC("name"));
	case 2:
		return Text::StrConcatC(buff, UTF8STRC("type"));
	case 3:
		return Text::StrConcatC(buff, UTF8STRC("geometry"));
	default:
		return nullptr;
	}
}

DB::DBUtil::ColType Map::OSM::OSMData::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const
{
	switch (colIndex)
	{
	case 0:
		colSize.Set(21);
		return DB::DBUtil::ColType::CT_Int64;
	case 1:
		colSize.Set(65536);
		return DB::DBUtil::ColType::CT_VarUTF8Char;
	case 2:
		colSize.Set(65536);
		return DB::DBUtil::ColType::CT_VarUTF8Char;
	case 3:
		colSize.Set(65536);
		return DB::DBUtil::ColType::CT_Vector;
	default:
		return DB::DBUtil::ColType::CT_Unknown;
	}
}

Bool Map::OSM::OSMData::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const
{
	switch (colIndex)
	{
	case 0:
		colDef->SetColName(CSTR("id"));
		colDef->SetColType(DB::DBUtil::ColType::CT_Int64);
		colDef->SetColSize(21);
		colDef->SetNotNull(true);
		colDef->SetPK(true);
		colDef->SetNativeType(CSTR("bigint"));
		return true;		
	case 1:
		colDef->SetColName(CSTR("name"));
		colDef->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
		colDef->SetColSize(65536);
		colDef->SetNotNull(false);
		colDef->SetNativeType(CSTR("varchar"));
		return true;
	case 2:
		colDef->SetColName(CSTR("type"));
		colDef->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
		colDef->SetColSize(65536);
		colDef->SetNotNull(false);
		colDef->SetNativeType(CSTR("varchar"));
		return true;
	case 3:
		colDef->SetColName(CSTR("geometry"));
		colDef->SetColType(DB::DBUtil::ColType::CT_Vector);
		colDef->SetColSize(65536);
		colDef->SetNotNull(false);
		colDef->SetNativeType(CSTR("geometry"));
		colDef->SetGeometrySRID(4326);
		return true;	
	default:
		return false;
	}
	return false;
}

UInt32 Map::OSM::OSMData::GetCodePage() const
{
	return 65001;
}

Bool Map::OSM::OSMData::GetBounds(OutParam<Math::RectAreaDbl> rect) const
{
	rect.Set(this->bounds);
	return true;
}

NN<Map::GetObjectSess> Map::OSM::OSMData::BeginGetObject()
{
	return NN<Map::GetObjectSess>::FromPtr((Map::GetObjectSess*)-1);
}

void Map::OSM::OSMData::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::OSM::OSMData::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<ElementInfo> elem;
	if (!this->GetElementById(id >> 2, (ElementType)(id & 3)).SetTo(elem))
	{
		return nullptr;
	}
	return this->CreateVector(elem);
}

UOSInt Map::OSM::OSMData::GetGeomCol() const
{
	return 3;
}

Map::MapDrawLayer::ObjectClass Map::OSM::OSMData::GetObjectClass() const
{
	return ObjectClass::OC_OSMDATA;
}

Text::CStringNN Map::OSM::ElementTypeGetName(ElementType type)
{
	switch (type)
	{
	case Map::OSM::ElementType::Node:
		return CSTR("node");
	case Map::OSM::ElementType::Way:
		return CSTR("way");
	case Map::OSM::ElementType::Relation:
		return CSTR("relation");
	case Map::OSM::ElementType::Unknown:
	default:
		return CSTR("unknown");
	}
}

Map::OSM::ElementType Map::OSM::ElementTypeFromString(Text::CStringNN str)
{
	if (str.Equals(UTF8STRC("node")))
	{
		return Map::OSM::ElementType::Node;
	}
	else if (str.Equals(UTF8STRC("way")))
	{
		return Map::OSM::ElementType::Way;
	}
	else if (str.Equals(UTF8STRC("relation")))
	{
		return Map::OSM::ElementType::Relation;
	}
	else
	{
		return Map::OSM::ElementType::Unknown;
	}
}

Text::CStringNN Map::OSM::LayerTypeGetName(LayerType val)
{
	switch (val)
	{
	default:
	case LayerType::Unknown:
		return CSTR("Unknown");
	case LayerType::Aeroway:
		return CSTR("Aeroway");
	case LayerType::Amenity:
		return CSTR("Amenity");
	case LayerType::AmenityCafe:
		return CSTR("AmenityCafe");
	case LayerType::AmenityFastFood:
		return CSTR("AmenityFastFood");
	case LayerType::AmenityKindergarten:
		return CSTR("AmenityKindergarten");
	case LayerType::AmenityMarketplace:
		return CSTR("AmenityMarketplace");
	case LayerType::AmenityPolice:
		return CSTR("AmenityPolice");
	case LayerType::AmenityPostBox:
		return CSTR("AmenityPostBox");
	case LayerType::AmenityRestaurant:
		return CSTR("AmenityRestaurant");
	case LayerType::AmenityTaxi:
		return CSTR("AmenityTaxi");
	case LayerType::AmenityTelephone:
		return CSTR("AmenityTelephone");
	case LayerType::Boundary:
		return CSTR("Boundary");
	case LayerType::Building:
		return CSTR("Building");
	case LayerType::Golf:
		return CSTR("Golf");
	case LayerType::GolfBunker:
		return CSTR("GolfBunker");
	case LayerType::GolfFairway:
		return CSTR("GolfFairway");
	case LayerType::GolfHole:
		return CSTR("GolfHole");
	case LayerType::GolfRough:
		return CSTR("GolfRough");
	case LayerType::Highway:
		return CSTR("Highway");
	case LayerType::HighwayBusStop:
		return CSTR("HighwayBusStop");
	case LayerType::HighwayConstruction:
		return CSTR("HighwayConstruction");
	case LayerType::HighwayCorridor:
		return CSTR("HighwayCorridor");
	case LayerType::HighwayCrossing:
		return CSTR("HighwayCrossing");
	case LayerType::HighwayCycleway:
		return CSTR("HighwayCycleway");
	case LayerType::HighwayElevator:
		return CSTR("HighwayElevator");
	case LayerType::HighwayEmergencyAccessPoint:
		return CSTR("HighwayEmergencyAccessPoint");
	case LayerType::HighwayFootway:
		return CSTR("HighwayFootway");
	case LayerType::HighwayGiveWay:
		return CSTR("HighwayGiveWay");
	case LayerType::HighwayMilestone:
		return CSTR("HighwayMilestone");
	case LayerType::HighwayMotorway:
		return CSTR("HighwayMotorway");
	case LayerType::HighwayMotorwayLink:
		return CSTR("HighwayMotorwayLink");
	case LayerType::HighwayPath:
		return CSTR("HighwayPath");
	case LayerType::HighwayPedestrian:
		return CSTR("HighwayPedestrian");
	case LayerType::HighwayPlatform:
		return CSTR("HighwayPlatform");
	case LayerType::HighwayPrimary:
		return CSTR("HighwayPrimary");
	case LayerType::HighwayPrimaryLink:
		return CSTR("HighwayPrimaryLink");
	case LayerType::HighwayResidential:
		return CSTR("HighwayResidential");
	case LayerType::HighwayRestArea:
		return CSTR("HighwayRestArea");
	case LayerType::HighwaySecondary:
		return CSTR("HighwaySecondary");
	case LayerType::HighwaySecondaryLink:
		return CSTR("HighwaySecondaryLink");
	case LayerType::HighwayService:
		return CSTR("HighwayService");
	case LayerType::HighwaySpeedCamera:
		return CSTR("HighwaySpeedCamera");
	case LayerType::HighwaySteps:
		return CSTR("HighwaySteps");
	case LayerType::HighwayStreetLamp:
		return CSTR("HighwayStreetLamp");
	case LayerType::HighwayTertiary:
		return CSTR("HighwayTertiary");
	case LayerType::HighwayTrafficSignals:
		return CSTR("HighwayTrafficSignals");
	case LayerType::HighwayTrunk:
		return CSTR("HighwayTrunk");
	case LayerType::HighwayTrunkLink:
		return CSTR("HighwayTrunkLink");
	case LayerType::HighwayUnclassified:
		return CSTR("HighwayUnclassified");
	case LayerType::Landuse:
		return CSTR("Landuse");
	case LayerType::LanduseIndustrial:
		return CSTR("LanduseIndustrial");
	case LayerType::LanduseResidential:
		return CSTR("LanduseResidential");
	case LayerType::Leisure:
		return CSTR("Leisure");
	case LayerType::LeisurePark:
		return CSTR("LeisurePark");
	case LayerType::LeisurePitch:
		return CSTR("LeisurePitch");
	case LayerType::Natural:
		return CSTR("Natural");
	case LayerType::NaturalBay:
		return CSTR("NaturalBay");
	case LayerType::NaturalCape:
		return CSTR("NaturalCape");
	case LayerType::NaturalGrassland:
		return CSTR("NaturalGrassland");
	case LayerType::NaturalPeak:
		return CSTR("NaturalPeak");
	case LayerType::NaturalRidge:
		return CSTR("NaturalRidge");
	case LayerType::NaturalRock:
		return CSTR("NaturalRock");
	case LayerType::NaturalSaddle:
		return CSTR("NaturalSaddle");
	case LayerType::NaturalScrub:
		return CSTR("NaturalScrub");
	case LayerType::NaturalValley:
		return CSTR("NaturalValley");
	case LayerType::NaturalWood:
		return CSTR("NaturalWood");
	case LayerType::Place:
		return CSTR("Place");
	case LayerType::PlaceHamlet:
		return CSTR("PlaceHamlet");
	case LayerType::PlaceLocality:
		return CSTR("PlaceLocality");
	case LayerType::Power:
		return CSTR("Power");
	case LayerType::PowerLine:
		return CSTR("PowerLine");
	case LayerType::PowerSubstation:
		return CSTR("PowerSubstation");
	case LayerType::Railway:
		return CSTR("Railway");
	case LayerType::Road:
		return CSTR("Road");
	case LayerType::Transport:
		return CSTR("Transport");
	case LayerType::Waterway:
		return CSTR("Waterway");
	case LayerType::WaterwayStream:
		return CSTR("WaterwayStream");
	case LayerType::Count:
		return CSTR("Count");
	}
}
