#include "Stdafx.h"
#include "DB/TableDef.h"
#include "Map/GeoPackageLayer.h"
#include "Math/CoordinateSystemManager.h"

Map::GeoPackageLayer::GeoPackageLayer(Map::GeoPackage *gpkg, Map::GeoPackage::ContentInfo *layerContent) : Map::MapDrawLayer(gpkg->GetSourceNameObj(), 0, layerContent->tableName)
{
	this->gpkg = gpkg;
	this->layerContent = layerContent;
	this->tabDef = this->gpkg->GetTableDef(CSTR_NULL, layerContent->tableName->ToCString());
	this->geomCol = INVALID_INDEX;
	this->mixedData = MixedData::AllData;
	this->SetCoordinateSystem(Math::CoordinateSystemManager::SRCreateCSys((UInt32)this->layerContent->srsId));
	if (this->tabDef)
	{
		DB::ColDef *col;
		UOSInt i = this->tabDef->GetColCnt();
		while (i-- > 0)
		{
			col = this->tabDef->GetCol(i);
			if (col->GetColType() == DB::DBUtil::CT_Vector)
			{
				this->geomCol = i;
				break;
			}
		}

		if (this->geomCol != INVALID_INDEX)
		{
			DB::DBReader *r = this->gpkg->QueryTableData(CSTR_NULL, this->layerContent->tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
			if (r)
			{
				while (r->ReadNext())
				{
					this->vecList.Add(r->GetVector(this->geomCol));
				}
				this->gpkg->CloseReader(r);
			}
		}
	}
}

Map::GeoPackageLayer::~GeoPackageLayer()
{
	this->gpkg->Release();
	SDEL_CLASS(this->tabDef);
	Math::Geometry::Vector2D *vec;
	UOSInt i = this->vecList.GetCount();
	while (i-- > 0)
	{
		vec = this->vecList.GetItem(i);
		SDEL_CLASS(vec);
	}
}

Map::DrawLayerType Map::GeoPackageLayer::GetLayerType()
{
	///////////////////////
	return Map::DRAW_LAYER_MIXED;	
}

void Map::GeoPackageLayer::SetMixedData(MixedData mixedData)
{
	this->mixedData = mixedData;
}

UOSInt Map::GeoPackageLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr)
{
	if (nameArr) *nameArr = 0;
	UOSInt i;
	UOSInt j;
	UOSInt initCnt;
	Math::Geometry::Vector2D *vec;
	switch (this->mixedData)
	{
	default:
	case MixedData::AllData:
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			outArr->Add((Int64)i);
			i++;
		}
		return j;
	case MixedData::PointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	case MixedData::NonPointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && !Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	}
}

UOSInt Map::GeoPackageLayer::GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() * mapRate, keepEmpty);
}

UOSInt Map::GeoPackageLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	if (nameArr) *nameArr = 0;
	UOSInt i;
	UOSInt j;
	UOSInt initCnt;
	Math::Geometry::Vector2D *vec;
	switch (this->mixedData)
	{
	default:
	case MixedData::AllData:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && rect.OverlapOrTouch(vec->GetBounds()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	case MixedData::PointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) && rect.OverlapOrTouch(vec->GetBounds()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	case MixedData::NonPointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && !Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) && rect.OverlapOrTouch(vec->GetBounds()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	}
}

Int64 Map::GeoPackageLayer::GetObjectIdMax()
{
	return (Int64)(this->vecList.GetCount() - 1);
}

void Map::GeoPackageLayer::ReleaseNameArr(NameArray *nameArr)
{
	////////////////////////////
}

UTF8Char *Map::GeoPackageLayer::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	////////////////////////////
	return 0;
}

UOSInt Map::GeoPackageLayer::GetColumnCnt()
{
	if (this->tabDef == 0)	return 0;
	return this->tabDef->GetColCnt();
}

UTF8Char *Map::GeoPackageLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (this->tabDef == 0)
		return 0;
	DB::ColDef *col = this->tabDef->GetCol(colIndex);
	if (col == 0)
		return 0;
	return col->GetColName()->ConcatTo(buff);
}

DB::DBUtil::ColType Map::GeoPackageLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (this->tabDef == 0)
		return DB::DBUtil::CT_Unknown;
	DB::ColDef *col = this->tabDef->GetCol(colIndex);
	if (col == 0)
		return DB::DBUtil::CT_Unknown;
	if (colSize)
		*colSize = col->GetColSize();
	return col->GetColType();
}

Bool Map::GeoPackageLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (this->tabDef == 0)
		return false;
	DB::ColDef *col = this->tabDef->GetCol(colIndex);
	if (col == 0)
		return false;
	colDef->Set(col);
	return true;
}

UInt32 Map::GeoPackageLayer::GetCodePage()
{
	return 65001;
}

Bool Map::GeoPackageLayer::GetBounds(Math::RectAreaDbl *rect)
{
	*rect = this->layerContent->bounds;
	return true;
}

Map::GetObjectSess *Map::GeoPackageLayer::BeginGetObject()
{
	return (Map::GetObjectSess*)-1;
}

void Map::GeoPackageLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::GeoPackageLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	Math::Geometry::Vector2D *vec = this->vecList.GetItem((UOSInt)id);
	if (vec)
		return vec->Clone();
	return 0;
}

UOSInt Map::GeoPackageLayer::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	return this->gpkg->QueryTableNames(schemaName, names);
}

DB::DBReader *Map::GeoPackageLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return this->gpkg->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
}

DB::TableDef *Map::GeoPackageLayer::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return this->gpkg->GetTableDef(schemaName, tableName);
}

void Map::GeoPackageLayer::CloseReader(DB::DBReader *r)
{
	this->gpkg->CloseReader(r);
}

void Map::GeoPackageLayer::GetLastErrorMsg(Text::StringBuilderUTF8 *str)
{
	this->gpkg->GetLastErrorMsg(str);
}

void Map::GeoPackageLayer::Reconnect()
{
	this->gpkg->Reconnect();
}

Map::MapDrawLayer::ObjectClass Map::GeoPackageLayer::GetObjectClass()
{
	return Map::MapDrawLayer::ObjectClass::OC_GEOPACKAGE;
}