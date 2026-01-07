#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
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
		tags->Clear();
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
}

Map::OSM::OSMData::~OSMData()
{
	OPTSTR_DEL(this->lang);
	OPTSTR_DEL(this->note);
	this->eleGroups.DeleteAll();
	this->elements.FreeAll(Map::OSM::OSMData::FreeElement);
}

IO::ParserType Map::OSM::OSMData::GetParserType() const
{
	return IO::ParserType::OSMData;
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
	grp->elements.Put(id, node);

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
	Int32 x;
	Int32 y;
	x = (Int32)(node->lon / this->groupDist);
	y = (Int32)(node->lat / this->groupDist);
	NN<ElementGroup> grp;
	if (!this->eleGroups.Get(x << 16 | y).SetTo(grp))
	{
		NEW_CLASSNN(grp, ElementGroup());
		grp->x = x;
		grp->y = y;
		this->eleGroups.Put(x << 16 | y, grp);
	}
	grp->elements.Put(way->id, way);
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

void Map::OSM::OSMData::SortElements()
{
	ElementComparator cmp;
	Data::Sort::ArtificialQuickSort::Sort<NN<ElementInfo>>(this->elements, cmp);
	this->nodeSorted = true;
	this->waySorted = true;
	this->relationSorted = true;
}

Map::DrawLayerType Map::OSM::OSMData::GetLayerType() const
{
	return Map::DrawLayerType::DRAW_LAYER_MIXED;
}

UOSInt Map::OSM::OSMData::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	NN<ElementInfo> elem;
	UOSInt i = 0;
	UOSInt j = this->elements.GetCount();
	while (i < j)
	{
		elem = this->elements.GetItemNoCheck(i);
		outArr->Add(elem->id << 2 | (UInt8)elem->type);
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
	UOSInt ret = 0;
	NN<ElementInfo> elem;
	Int32 x;
	Int32 y;
	x = minX;
	while (x <= maxX)
	{
		y = minY;
		while (y <= maxY)
		{
			NN<ElementGroup> grp;
			if (this->eleGroups.Get(x << 16 | y).SetTo(grp))
			{
				UOSInt k = 0;
				UOSInt l = grp->elements.GetCount();
				while (k < l)
				{
					elem = grp->elements.GetItemNoCheck(k);
					if (!elem->hasParent)
					{
						outArr->Add(elem->id << 2 | (UInt8)elem->type);
					}
					k++;
				}
				ret += l;
			}
			y++;
		}
		x++;
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
