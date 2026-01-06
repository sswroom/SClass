#include "Stdafx.h"
#include "DB/ColDef.h"
#include "Map/OSM/OSMData.h"
#include "Math/CoordinateSystemManager.h"

void Map::OSM::OSMData::FreeElement(NN<ElementInfo> elem)
{
	NN<Data::ArrayListNN<TagInfo>> tags;
	if (elem->tags.SetTo(tags))
	{
		NN<TagInfo> tag;
		UOSInt i = tags->GetCount();
		NN<Text::String> value;
		while (i-- > 0)
		{
			tag = tags->GetItemNoCheck(i);
			tag->k->Release();
			tag->v->Release();
			MemFreeNN(tag);
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
		rel->members.Clear();
	}
	elem.Delete();
}

Map::OSM::OSMData::OSMData(Text::CStringNN sourceName) : Map::MapDrawLayer(sourceName, 0, CSTR("OSM"), Math::CoordinateSystemManager::CreateWGS84Csys())
{
}

Map::OSM::OSMData::~OSMData()
{
}

IO::ParserType Map::OSM::OSMData::GetParserType() const
{
	return IO::ParserType::OSMData;
}

NN<Map::OSM::NodeInfo> Map::OSM::OSMData::NewNode(Int64 id, Double lat, Double lon)
{
	NN<NodeInfo> node;
	NEW_CLASSNN(node, NodeInfo());
	node->id = id;
	node->type = ElementType::Node;
	node->lat = lat;
	node->lon = lon;
	this->elements.Put(id, node);
	return node;
}

NN<Map::OSM::WayInfo> Map::OSM::OSMData::NewWay(Int64 id)
{
	NN<WayInfo> way;
	NEW_CLASSNN(way, WayInfo());
	way->id = id;
	way->type = ElementType::Way;
	this->elements.Put(id, way);
	return way;
}

NN<Map::OSM::RelationInfo> Map::OSM::OSMData::NewRelation(Int64 id)
{
	NN<RelationInfo> rel;
	NEW_CLASSNN(rel, RelationInfo());
	rel->id = id;
	rel->type = ElementType::Relation;
	this->elements.Put(id, rel);
	return rel;
}

Map::DrawLayerType Map::OSM::OSMData::GetLayerType() const
{
	return Map::DrawLayerType::DRAW_LAYER_MIXED;
}

UOSInt Map::OSM::OSMData::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	return 0;
}

UOSInt Map::OSM::OSMData::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return 0;
}
UOSInt Map::OSM::OSMData::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	return 0;
}

Int64 Map::OSM::OSMData::GetObjectIdMax() const
{
	return 0;
}

UOSInt Map::OSM::OSMData::GetRecordCnt() const
{
	return 0;
}

void Map::OSM::OSMData::ReleaseNameArr(Optional<NameArray> nameArr)
{
}

Bool Map::OSM::OSMData::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	return false;
}

UOSInt Map::OSM::OSMData::GetColumnCnt() const
{
	return 0;
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
	return false;
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
	return 0;
}

UOSInt Map::OSM::OSMData::GetGeomCol() const
{
	return 3;
}

Map::MapDrawLayer::ObjectClass Map::OSM::OSMData::GetObjectClass() const
{
	return ObjectClass::OC_OSMDATA;
}
