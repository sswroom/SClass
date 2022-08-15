#include "Stdafx.h"
#include "Map/DBMapLayer.h"

Map::DBMapLayer::DBMapLayer(Text::String *layerName) : Map::IMapDrawLayer(layerName, 0, layerName)
{

}

Map::DBMapLayer::~DBMapLayer()
{

}

Map::DrawLayerType Map::DBMapLayer::GetLayerType()
{
	return Map::DRAW_LAYER_MIXED;
}

void Map::DBMapLayer::SetMixedType(Math::Geometry::Vector2D::VectorType mixedType)
{

}

UOSInt Map::DBMapLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{

}

UOSInt Map::DBMapLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{

}

UOSInt Map::DBMapLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{

}

Int64 Map::DBMapLayer::GetObjectIdMax()
{

}

void Map::DBMapLayer::ReleaseNameArr(void *nameArr)
{

}

UTF8Char *Map::DBMapLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{

}

UOSInt Map::DBMapLayer::GetColumnCnt()
{

}

UTF8Char *Map::DBMapLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{

}

DB::DBUtil::ColType Map::DBMapLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{

}

Bool Map::DBMapLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{

}

UInt32 Map::DBMapLayer::GetCodePage()
{
	return 65001;
}

Bool Map::DBMapLayer::GetBounds(Math::RectAreaDbl *rect)
{

}

void *Map::DBMapLayer::BeginGetObject()
{

}

void Map::DBMapLayer::EndGetObject(void *session)
{

}

Map::DrawObjectL *Map::DBMapLayer::GetNewObjectById(void *session, Int64 id)
{

}

Math::Geometry::Vector2D *Map::DBMapLayer::GetNewVectorById(void *session, Int64 id)
{

}

void Map::DBMapLayer::ReleaseObject(void *session, DrawObjectL *obj)
{

}

UOSInt Map::DBMapLayer::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{

}

DB::DBReader *Map::DBMapLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{

}

void Map::DBMapLayer::CloseReader(DB::DBReader *r)
{

}

void Map::DBMapLayer::GetErrorMsg(Text::StringBuilderUTF8 *str)
{

}

void Map::DBMapLayer::Reconnect()
{

}

Map::IMapDrawLayer::ObjectClass Map::DBMapLayer::GetObjectClass()
{
	return OC_DB_MAP_LAYER;
}
